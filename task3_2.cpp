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

    // Требуемая точность
    constexpr double EPS  = 1e-7;
    constexpr double EPS2 = EPS * EPS;

    for (size_t k = 0; k < points.size(); ++k) {
        double px = static_cast<double>(points[k].x);
        double py = static_cast<double>(points[k].y);

        int winding_number = 0;
        bool onBoundary = false;

        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            double x1 = static_cast<double>(polygon[i].x);
            double y1 = static_cast<double>(polygon[i].y);
            double x2 = static_cast<double>(polygon[j].x);
            double y2 = static_cast<double>(polygon[j].y);

            // --- 1. Проверка границы (ребро или вершина) ---
            double dx = x2 - x1;
            double dy = y2 - y1;
            double len2 = dx * dx + dy * dy;

            if (len2 > 1e-12) {
                // Проекция точки на прямую ребра
                double t = ((px - x1) * dx + (py - y1) * dy) / len2;
                t = (t < 0.0) ? 0.0 : ((t > 1.0) ? 1.0 : t); // Clamp to [0,1]
                
                double cx = x1 + t * dx;
                double cy = y1 + t * dy;
                double dist2 = (px - cx) * (px - cx) + (py - cy) * (py - cy);

                if (dist2 <= EPS2) {
                    onBoundary = true;
                    break; // Точка на границе -> сразу считаем входящей
                }
            } else {
                // Вырожденное ребро (совпадающие вершины)
                double dist2 = (px - x1) * (px - x1) + (py - y1) * (py - y1);
                if (dist2 <= EPS2) {
                    onBoundary = true;
                    break;
                }
            }

            // --- 2. Winding Number (Non-Zero Rule) ---
            // Пересечение вверх
            if (y1 <= py && y2 > py) {
                // Точка слева от вектора (x1,y1)->(x2,y2)?
                if ((px - x1) * (y2 - y1) < (py - y1) * (x2 - x1)) {
                    winding_number++;
                }
            }
            // Пересечение вниз
            else if (y1 > py && y2 <= py) {
                // Точка справа от вектора?
                if ((px - x1) * (y2 - y1) > (py - y1) * (x2 - x1)) {
                    winding_number--;
                }
            }
        }

        result[k] = (onBoundary || winding_number != 0) ? 1 : 0;
    }
}
