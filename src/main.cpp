#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

struct Point {
    double x;
    double y;
};

static auto dist(const Point& a, const Point& b) -> double {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt((dx * dx) + (dy * dy));
}

struct MstResult {
    int64_t total_length;
    std::vector<int32_t> parent;
    std::vector<int64_t> edge_length;
};

static auto compute_mst(const std::vector<Point>& pts) -> MstResult {
    const size_t n = pts.size();
    MstResult res{};
    res.total_length = 0;
    res.parent.assign(n, -1);
    res.edge_length.assign(n, 0LL);

    if (n <= 1)
        return res;

    std::vector<bool> in_mst(n, false);
    std::vector<double> min_edge(n, std::numeric_limits<double>::max());

    min_edge[0] = 0.0;

    for (size_t i = 0; i < n; i++) {
        double best = std::numeric_limits<double>::max();
        size_t u = n;

        for (size_t j = 0; j < n; j++) {
            if (!in_mst[j] && min_edge[j] < best) {
                best = min_edge[j];
                u = j;
            }
        }

        if (u == n)
            break;

        in_mst[u] = true;

        if (res.parent[u] != -1) {
            const double d
                = dist(pts[u], pts[static_cast<size_t>(res.parent[u])]);
            const auto w = static_cast<int64_t>(std::llround(d * 1000.0));
            res.total_length += w;
            res.edge_length[u] = w;
        }

        for (size_t v = 0; v < n; v++) {
            if (!in_mst[v]) {
                const double d = dist(pts[u], pts[v]);
                if (d < min_edge[v]) {
                    min_edge[v] = d;
                    res.parent[v] = static_cast<int32_t>(u);
                }
            }
        }
    }

    return res;
}

static auto format_with_commas(int64_t value) -> std::string {
    std::string s = std::to_string(value);
    std::string out;
    int32_t count = 0;

    for (auto it = s.rbegin(); it != s.rend(); it++) {
        if (count == 3) {
            out.push_back(',');
            count = 0;
        }
        out.push_back(*it);
        ++count;
    }

    std::reverse(out.begin(), out.end());
    return out;
}

auto main(int32_t argc, char* argv[]) -> int32_t {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <platforms_file> <stations_file>\n";
        return -1;
    }

    std::ifstream pf(argv[1]);
    if (!pf) {
        std::cerr << "Failed to open platforms file: " << argv[1] << '\n';
        return -1;
    }

    std::ifstream sf(argv[2]);
    if (!sf) {
        std::cerr << "Failed to open stations file: " << argv[2] << '\n';
        return -1;
    }

    std::vector<Point> platforms;

    double x; // NOLINT(cppcoreguidelines-init-variables)
    double y; // NOLINT(cppcoreguidelines-init-variables)

    while (pf >> x >> y)
        platforms.push_back(Point{x, y});

    std::vector<Point> stations;

    while (sf >> x >> y)
        stations.push_back(Point{x, y});

    if (platforms.empty() || stations.empty()) {
        std::cerr << "No platforms or no stations loaded.\n";
        return -1;
    }

    for (size_t i = 0; i < stations.size(); i++) {
        std::vector<Point> all = platforms;
        all.push_back(stations[i]);

        const MstResult mst = compute_mst(all);

        const int64_t total_rounded = mst.total_length;

        std::cout << "Pumping Station " << i << ":\n";
        std::cout << "Total Pipeline Length: "
                  << format_with_commas(total_rounded) << '\n';

        struct EdgeInfo {
            int32_t a;
            int32_t b;
            int64_t length;
        };

        std::vector<EdgeInfo> edges;
        edges.reserve(all.size() > 0 ? all.size() - 1 : 0);

        for (size_t v = 0; v < all.size(); v++) {
            const int32_t parent_idx = mst.parent[v];
            if (parent_idx < 0)
                continue;
            const int32_t a = std::min(parent_idx, static_cast<int32_t>(v));
            const int32_t b = std::max(parent_idx, static_cast<int32_t>(v));
            edges.push_back(EdgeInfo{a, b, mst.edge_length[v]});
        }

        std::sort(
            edges.begin(),
            edges.end(),
            [](const EdgeInfo& lhs, const EdgeInfo& rhs) {
                return lhs.length < rhs.length;
            }
        );

        for (const auto& e : edges) {
            std::cout << "  From Point " << e.a << " to Point " << e.b
                      << ": Length " << format_with_commas(e.length) << '\n';
        }

        std::cout << '\n';
    }
}
