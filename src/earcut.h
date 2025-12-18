#include <vector>
#include <cmath>
#include <cstdint>
#include <limits>
#include <algorithm>

#include <glm/vec2.hpp> // glm::dvec2

static constexpr double EPS = 1e-12;

static inline double cross(const glm::dvec2& a, const glm::dvec2& b, const glm::dvec2& c) {
    // cross((b-a),(c-a))
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

static double signed_area(const std::vector<glm::dvec2>& ring) {
    double a = 0.0;
    const int n = (int)ring.size();
    for (int i = 0; i < n; ++i) {
        const auto& p = ring[i];
        const auto& q = ring[(i + 1) % n];
        a += p.x * q.y - q.x * p.y;
    }
    return 0.5 * a;
}

static void ensure_winding(std::vector<glm::dvec2>& ring, bool wantCCW) {
    if (ring.size() < 3) return;
    bool isCCW = signed_area(ring) > 0.0;
    if (isCCW != wantCCW) std::reverse(ring.begin(), ring.end());
}

static bool point_in_triangle(
    const glm::dvec2& p,
    const glm::dvec2& a,
    const glm::dvec2& b,
    const glm::dvec2& c)
{
    // Oriented area tests (accept points on edges)
    double c1 = cross(a, b, p);
    double c2 = cross(b, c, p);
    double c3 = cross(c, a, p);
    bool has_neg = (c1 < -EPS) || (c2 < -EPS) || (c3 < -EPS);
    bool has_pos = (c1 > EPS) || (c2 > EPS) || (c3 > EPS);
    return !(has_neg && has_pos);
}

// Segment intersection (excluding shared endpoints) for conservative visibility test
static bool seg_intersect_strict(
    const glm::dvec2& a, const glm::dvec2& b,
    const glm::dvec2& c, const glm::dvec2& d)
{
    auto orient = [](const glm::dvec2& p, const glm::dvec2& q, const glm::dvec2& r) {
        double v = cross(p, q, r);
        if (std::abs(v) < EPS) return 0;
        return (v > 0) ? 1 : -1;
        };

    int o1 = orient(a, b, c);
    int o2 = orient(a, b, d);
    int o3 = orient(c, d, a);
    int o4 = orient(c, d, b);

    if (o1 == 0 || o2 == 0 || o3 == 0 || o4 == 0) {
        // Treat colinear/touching as intersecting for conservative visibility
        return true;
    }
    return (o1 != o2) && (o3 != o4);
}

// Merge one hole into polygon by bridging:
// - pick rightmost vertex of hole (max x, tie min y)
// - pick polygon vertex to connect to (nearest that doesn't cross polygon edges)
//
// NOTE: This is a simplified bridging strategy that works well for clean, Doom-like input.
static void bridge_hole_into_polygon(
    std::vector<glm::dvec2>& poly,
    const std::vector<glm::dvec2>& hole)
{
    // 1) rightmost hole vertex
    int hr = 0;
    for (int i = 1; i < (int)hole.size(); ++i) {
        if (hole[i].x > hole[hr].x + EPS ||
            (std::abs(hole[i].x - hole[hr].x) <= EPS && hole[i].y < hole[hr].y))
        {
            hr = i;
        }
    }
    const glm::dvec2 P = hole[hr];

    // 2) choose a polygon vertex Q to connect to
    int best_q = -1;
    double best_dist2 = std::numeric_limits<double>::infinity();

    for (int qi = 0; qi < (int)poly.size(); ++qi) {
        const glm::dvec2& Q = poly[qi];
        double dx = Q.x - P.x;
        double dy = Q.y - P.y;
        double dist2 = dx * dx + dy * dy;
        if (dist2 >= best_dist2) continue;

        // Check bridge doesn't cross existing polygon edges (conservative)
        bool ok = true;
        const int n = (int)poly.size();
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            if (i == qi || j == qi) continue; // skip edges incident to Q
            if (seg_intersect_strict(P, Q, poly[i], poly[j])) { ok = false; break; }
        }
        if (!ok) continue;

        best_q = qi;
        best_dist2 = dist2;
    }

    if (best_q < 0) {
        // Fallback: nearest vertex ignoring visibility (should be rare for clean input)
        for (int qi = 0; qi < (int)poly.size(); ++qi) {
            double dx = poly[qi].x - P.x;
            double dy = poly[qi].y - P.y;
            double dist2 = dx * dx + dy * dy;
            if (dist2 < best_dist2) { best_dist2 = dist2; best_q = qi; }
        }
    }

    // 3) splice hole into poly at best_q
    std::vector<glm::dvec2> out;
    out.reserve(poly.size() + hole.size() + 2);

    // Copy poly up to and including Q
    for (int i = 0; i <= best_q; ++i) out.push_back(poly[i]);

    // Insert bridge Q -> P
    out.push_back(P);

    // Insert hole vertices from hr+1..end..0..hr (forward order)
    for (int k = 1; k < (int)hole.size(); ++k) {
        out.push_back(hole[(hr + k) % (int)hole.size()]);
    }

    // Close bridge back via repeating P
    out.push_back(P);

    // Continue poly
    for (int i = best_q + 1; i < (int)poly.size(); ++i) out.push_back(poly[i]);

    poly.swap(out);
}

// Ear clipping for a simple polygon
static bool earclip_simple_polygon(
    const std::vector<glm::dvec2>& poly,
    std::vector<uint32_t>& outIndices)
{
    const int n0 = (int)poly.size();
    if (n0 < 3) return false;

    std::vector<int> V(n0);
    for (int i = 0; i < n0; ++i) V[i] = i;

    const bool isCCW = signed_area(poly) > 0.0;

    auto is_convex = [&](const glm::dvec2& a, const glm::dvec2& b, const glm::dvec2& c) {
        double z = cross(a, b, c);
        return isCCW ? (z > EPS) : (z < -EPS);
        };

    int guard = 0;
    while ((int)V.size() > 3 && guard < 1000000) {
        ++guard;
        bool clipped = false;

        const int m = (int)V.size();
        for (int i = 0; i < m; ++i) {
            int i0 = V[(i + m - 1) % m];
            int i1 = V[i];
            int i2 = V[(i + 1) % m];

            const auto& a = poly[i0];
            const auto& b = poly[i1];
            const auto& c = poly[i2];

            if (!is_convex(a, b, c)) continue;

            bool any_inside = false;
            for (int j = 0; j < m; ++j) {
                int ij = V[j];
                if (ij == i0 || ij == i1 || ij == i2) continue;
                if (point_in_triangle(poly[ij], a, b, c)) {
                    any_inside = true;
                    break;
                }
            }
            if (any_inside) continue;

            // Output ear triangle
            if (isCCW) {
                outIndices.push_back((uint32_t)i0);
                outIndices.push_back((uint32_t)i1);
                outIndices.push_back((uint32_t)i2);
            }
            else {
                outIndices.push_back((uint32_t)i2);
                outIndices.push_back((uint32_t)i1);
                outIndices.push_back((uint32_t)i0);
            }

            V.erase(V.begin() + i);
            clipped = true;
            break;
        }

        if (!clipped) return false; // degeneracy/self-intersection likely
    }

    if ((int)V.size() == 3) {
        if (isCCW) {
            outIndices.push_back((uint32_t)V[0]);
            outIndices.push_back((uint32_t)V[1]);
            outIndices.push_back((uint32_t)V[2]);
        }
        else {
            outIndices.push_back((uint32_t)V[2]);
            outIndices.push_back((uint32_t)V[1]);
            outIndices.push_back((uint32_t)V[0]);
        }
        return true;
    }

    return false;
}

struct Triangulation2D {
    std::vector<glm::dvec2> vertices;
    std::vector<uint32_t> indices;
};

// Public API: outer ring + hole rings (each ring NOT explicitly closed)
Triangulation2D EarclipWithHoles(
    std::vector<glm::dvec2> outer,
    std::vector<std::vector<glm::dvec2>> holes)
{
    Triangulation2D result;
    if (outer.size() < 3) return result;

    // Normalize winding: outer CCW, holes CW
    ensure_winding(outer, true);
    for (auto& h : holes) {
        if (h.size() >= 3) ensure_winding(h, false);
    }

    std::vector<glm::dvec2> poly = std::move(outer);

    // Bridge each hole into the polygon
    for (const auto& h : holes) {
        if (h.size() < 3) continue;
        bridge_hole_into_polygon(poly, h);
    }

    // Ear-clip the simple polygon
    result.vertices = std::move(poly);
    if (!earclip_simple_polygon(result.vertices, result.indices)) {
        result.indices.clear(); // signal failure
    }
    return result;
}
