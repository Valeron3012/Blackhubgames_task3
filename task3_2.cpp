#include <vector>
#include <cmath>
#include <algorithm>

// struct Point { float x = 0.f; float y = 0.f; }; // Предоставлено в условии

void TestPoints(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    std::vector<int>& result)
{
    const size_t n = polygon.size();
    result.resize(points.size());
    
    if (n < 3) {
        std::fill(result.begin(), result.end(), 0);
        return;
    }

    // Точность 1e-7, как требуется в условии
    constexpr double EPS  = 1e-7;
    constexpr double EPS2 = EPS * EPS;

    for (size_t k = 0; k < points.size(); ++k) {
        // Работаем в double, чтобы избежать потери точности float
        double px = static_cast<double>(points[k].x);
        double py = static_cast<double>(points[k].y);

        bool inside = false;
        bool onEdge = false;

        // Один проход по всем рёбрам
        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            double x1 = static_cast<double>(polygon[i].x);
            double y1 = static_cast<double>(polygon[i].y);
            double x2 = static_cast<double>(polygon[j].x);
            double y2 = static_cast<double>(polygon[j].y);

            // 1. Явная проверка расстояния до ребра с точностью EPS
            if (!onEdge) {
                double dx = x2 - x1;
                double dy = y2 - y1;
                double len2 = dx * dx + dy * dy;
                double t = 0.0;
                
                if (len2 > 1e-12) {
                    // Проекция точки на прямую, содержащую ребро
                    t = ((px - x1) * dx + (py - y1) * dy) / len2;
                    // Ограничиваем проекцию отрезком [0, 1]
                    t = (t < 0.0) ? 0.0 : ((t > 1.0) ? 1.0 : t);
                }
                
                double closestX = x1 + t * dx;
                double closestY = y1 + t * dy;
                double dist2 = (px - closestX) * (px - closestX) + (py - closestY) * (py - closestY);

                if (dist2 <= EPS2) {
                    onEdge = true;
                }
            }

            // 2. Ray Casting (Even-Odd Rule)
            // Условие (y1 > py) != (y2 > py) математически корректно обрабатывает вершины:
            // луч считается "чуть выше" точки, что исключает двойной подсчёт на вершинах.
            if ((y1 > py) != (y2 > py)) {
                double xInter = (x2 - x1) * (py - y1) / (y2 - y1) + x1;
                if (px < xInter) {
                    inside = !inside;
                }
            }
        }

        // Точка внутри, если прошла проверку чётности ИЛИ лежит на границе в пределах EPS
        result[k] = (inside || onEdge) ? 1 : 0;
    }
}
