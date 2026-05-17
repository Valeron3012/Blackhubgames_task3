#include <vector>
#include <algorithm>

// struct Point { float x = 0.f; float y = 0.f; }; // Уже объявлен в условии

void TestPoints(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    std::vector<int>& result)
{
    const size_t n = polygon.size();
    const size_t m = points.size();
    result.resize(m);
    
    if (n < 3) {
        std::fill(result.begin(), result.end(), 0);
        return;
    }

    // 1. Вычисляем Bounding Box (O(N))
    float min_x = polygon[0].x, max_x = polygon[0].x;
    float min_y = polygon[0].y, max_y = polygon[0].y;
    for (size_t i = 1; i < n; ++i) {
        if (polygon[i].x < min_x) min_x = polygon[i].x;
        if (polygon[i].x > max_x) max_x = polygon[i].x;
        if (polygon[i].y < min_y) min_y = polygon[i].y;
        if (polygon[i].y > max_y) max_y = polygon[i].y;
    }

    constexpr double EPS2 = 1e-14; // (1e-7)^2 для сравнения квадратов расстояний

    // 2. Проверяем каждую точку
    for (size_t k = 0; k < m; ++k) {
        const Point& pt = points[k];
        
        // Быстрая отсечка
        if (pt.x < min_x || pt.x > max_x || pt.y < min_y || pt.y > max_y) {
            result[k] = 0;
            continue;
        }

        bool inside = false;
        bool onEdge = false;
        
        // Один проход по рёбрам: проверяем принадлежность границе + Ray Casting
        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const Point& vi = polygon[i];
            const Point& vj = polygon[j];

            // --- Проверка расстояния до ребра с точностью 1e-7 ---
            if (!onEdge) {
                double dx  = static_cast<double>(vj.x) - vi.x;
                double dy  = static_cast<double>(vj.y) - vi.y;
                double len2 = dx * dx + dy * dy;
                double dist2;

                if (len2 < 1e-12) { // Вырожденное ребро (точка)
                    dist2 = (static_cast<double>(pt.x) - vi.x) * (static_cast<double>(pt.x) - vi.x) +
                            (static_cast<double>(pt.y) - vi.y) * (static_cast<double>(pt.y) - vi.y);
                } else {
                    // Проекция точки на отрезок
                    double t = ((static_cast<double>(pt.x) - vi.x) * dx + 
                                (static_cast<double>(pt.y) - vi.y) * dy) / len2;
                    t = (t < 0.0) ? 0.0 : ((t > 1.0) ? 1.0 : t); // Clamp
                    
                    double cx = vi.x + t * dx;
                    double cy = vi.y + t * dy;
                    dist2 = (static_cast<double>(pt.x) - cx) * (static_cast<double>(pt.x) - cx) +
                            (static_cast<double>(pt.y) - cy) * (static_cast<double>(pt.y) - cy);
                }
                
                if (dist2 < EPS2) onEdge = true;
            }

            // --- Ray Casting (Division-Free) ---
            // Проверяем, пересекает ли ребро горизонтальный луч
            if ((vi.y > pt.y) != (vj.y > pt.y)) {
                double dy   = static_cast<double>(vj.y) - vi.y;
                double dx   = static_cast<double>(vj.x) - vi.x;
                double dy_pt = static_cast<double>(pt.y) - vi.y;
                double dx_pt = static_cast<double>(pt.x) - vi.x;

                // Умножение вместо деления для скорости и точности
                if (dy > 0.0) {
                    if (dx_pt * dy < dx * dy_pt) inside = !inside;
                } else {
                    if (dx_pt * dy > dx * dy_pt) inside = !inside;
                }
            }
        }
        
        // Точка внутри, если прошла Ray Casting ИЛИ лежит на границе с точностью 1e-7
        result[k] = (inside || onEdge) ? 1 : 0;
    }
}
