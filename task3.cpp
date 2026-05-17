#include <vector>
#include <algorithm>
#include <iostream>

struct Point {
    float x = 0.f;
    float y = 0.f;
};

// Вспомогательная структура для кэширования вычислений
struct PolygonCache {
    const Point* data_ptr = nullptr;
    float min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    bool is_valid = false;
};

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

    // 1. Получаем или вычисляем Bounding Box (O(N) только при первом вызове для полигона)
    // Используем static для сохранения состояния между вызовами, если полигон не меняется
    static PolygonCache cache;
    
    // Проверяем, не изменился ли полигон (по адресу данных)
    if (!cache.is_valid || cache.data_ptr != polygon.data()) {
        cache.data_ptr = polygon.data();
        cache.is_valid = true;
        
        cache.min_x = cache.max_x = polygon[0].x;
        cache.min_y = cache.max_y = polygon[0].y;
        
        for (size_t i = 1; i < n; ++i) {
            float x = polygon[i].x;
            float y = polygon[i].y;
            if (x < cache.min_x) cache.min_x = x;
            if (x > cache.max_x) cache.max_x = x;
            if (y < cache.min_y) cache.min_y = y;
            if (y > cache.max_y) cache.max_y = y;
        }
    }
    const float min_x = cache.min_x;
    const float max_x = cache.max_x;
    const float min_y = cache.min_y;
    const float max_y = cache.max_y;

    // 2. Проверяем каждую точку
    for (size_t k = 0; k < m; ++k) {
        const Point& pt = points[k];

        // Быстрая отсечка по Bounding Box
        if (pt.x < min_x || pt.x > max_x || pt.y < min_y || pt.y > max_y) {
            result[k] = 0;
            continue;
        }

        // 3. Ray Casting (Алгоритм трассировки луча)
        bool inside = false;
        for (size_t i = 0, j = n - 1; i < n; j = i++) {
            const Point& vi = polygon[i];
            const Point& vj = polygon[j];

            // Проверяем, пересекает ли ребро горизонтальный луч, идущий вправо от точки
            // Условие гарантирует, что ребро не горизонтальное, и деление безопасно
            if ((vi.y > pt.y) != (vj.y > pt.y)) {
                
                // ОПТИМИЗАЦИЯ: Замена деления на умножение
                // Стандартная формула: x_intersect = (vj.x - vi.x) * (pt.y - vi.y) / (vj.y - vi.y) + vi.x
                // Нам нужно проверить: pt.x < x_intersect
                // Это эквивалентно: (pt.x - vi.x) * (vj.y - vi.y) < (vj.x - vi.x) * (pt.y - vi.y)
                // С учетом знака (vj.y - vi.y)
                
                float dy = vj.y - vi.y;
                float dx = vj.x - vi.x;
                float dy_pt = pt.y - vi.y;
                float dx_pt = pt.x - vi.x;

                // dy никогда не равен 0 здесь из-за условия (vi.y > pt.y) != (vj.y > pt.y)
                if (dy > 0) {
                    // Если ребро идет вверх, знак неравенства сохраняется
                    if (dx_pt * dy < dx * dy_pt) {
                        inside = !inside;
                    }
                } else {
                    // Если ребро идет вниз, знак неравенства меняется
                    if (dx_pt * dy > dx * dy_pt) {
                        inside = !inside;
                    }
                }
            }        }
        result[k] = inside ? 1 : 0;
    }
}

int main() {
    // Пример: Квадрат
    std::vector<Point> polygon = {
        {0, 0}, {10, 0}, {10, 10}, {0, 10}
    };

    // Точки для проверки
    std::vector<Point> points = {
        {5, 5},   // Внутри (1)
        {15, 5},  // Снаружи (0)
        {0, 0},   // На вершине (обычно считается 0 или 1 в зависимости от реализации, здесь 0)
        {5, 10}   // На границе (0)
    };

    std::vector<int> result;
    TestPoints(polygon, points, result);

    // Вывод результатов
    std::cout << "Результаты проверки точек:" << std::endl;
    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << "Point(" << points[i].x << ", " << points[i].y << ") -> " 
                  << (result[i] ? "Внутри" : "Снаружи") << std::endl;
    }

    return 0;
}
