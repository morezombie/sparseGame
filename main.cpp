#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <regex>

// 输入
constexpr float threshold = 6.0f;
const static std::string input = R"(
const threshold = 6;
const input = [{
    x: 10,
    y: 60
}, {
    x: 12,
    y: 46
}, {
    x: 15,
    y: 36
}, {
    x: 23,
    y: 29
}, {
    x: 28,
    y: 34
}, {
    x: 30,
    y: 46
}, {
    x: 30,
    y: 56
}, {
    x: 27,
    y: 66
}, {
    x: 26,
    y: 76
}, {
    x: 30,
    y: 85
}, {
    x: 34,
    y: 87
}, {
    x: 40,
    y: 78
}, {
    x: 42,
    y: 72
}, {
    x: 43,
    y: 60
}, {
    x: 44,
    y: 51
}, {
    x: 46,
    y: 38
}, {
    x: 46,
    y: 26
}, {
    x: 49,
    y: 17
}, {
    x: 58,
    y: 11
}, {
    x: 63,
    y: 24
}, {
    x: 61,
    y: 38
}, {
    x: 60,
    y: 55
}, {
    x: 59,
    y: 67
}, {
    x: 58,
    y: 82
}, {
    x: 64,
    y: 90
}, {
    x: 68,
    y: 91
}, {
    x: 79,
    y: 89
}, {
    x: 84,
    y: 82
}, {
    x: 85,
    y: 70
}, {
    x: 83,
    y: 60
}, {
    x: 80,
    y: 50
}, {
    x: 79,
    y: 36
}, {
    x: 82,
    y: 26
}, {
    x: 86,
    y: 20
}, {
    x: 94,
    y: 17
}, {
    x: 113,
    y: 15
}, {
    x: 127,
    y: 19
}, {
    x: 133,
    y: 28
}, {
    x: 134,
    y: 40
}, {
    x: 134,
    y: 53
}, {
    x: 131,
    y: 62
}, {
    x: 124,
    y: 72
}, {
    x: 116,
    y: 76
}, {
    x: 105,
    y: 77
}, {
    x: 99,
    y: 73
}, {
    x: 94,
    y: 63
}, {
    x: 95,
    y: 50
}, {
    x: 102,
    y: 41
}, {
    x: 111,
    y: 34
}, {
    x: 123,
    y: 42
}, {
    x: 122,
    y: 53
}, {
    x: 116,
    y: 61
}, {
    x: 109,
    y: 64
}, {
    x: 106,
    y: 58
}]
    )";

// 抽稀类
class Sparser {
public:
struct Point {
    int x;
    int y;
};

void sparse(const std::string& json, std::string& sparseJson) {
    // 将json字符串转换为点的集合
    std::vector<Point> points;
    convertJsonToPoints(json, points);
    // 计算点的稀疏表示
    std::vector<Point> sparsePoints;
    calculateSparsePoints(points, sparsePoints);
    // 将稀疏点集合转换为json字符串
    convertPointsToJson(sparsePoints, sparseJson);
}

private:
void convertJsonToPoints(const std::string& json, std::vector<Point>& points) {
    // 使用正则表达式获取x和y的值
    std::regex regex(R"(\{\s*x:\s*(\d+),\s*y:\s*(\d+)\s*\})");
    std::smatch matches;
    std::string::const_iterator searchStart(json.cbegin());
    while (regex_search(searchStart, json.cend(), matches, regex)) {
        int x = std::stoi(matches[1].str());
        int y = std::stoi(matches[2].str());
        points.push_back(Point{x, y});
        searchStart = matches.suffix().first;
    }
}

void convertPointsToJson(const std::vector<Point>& points, std::string& json) {
    // 将points转换为字符串
    std::ostringstream ss;
    ss << "[ ";
    for (size_t i = 0; i < points.size(); i++) {
        ss << "[ " << points[i].x << ", " << points[i].y << " ]";
        if (i < points.size() - 1) {
            ss << ", ";
        }
    }
    ss << " ]";
    json = ss.str();
}

void calculateSparsePoints(const std::vector<Point>& points, std::vector<Point> &sparsePoints) {
    // 计算稀疏点
    std::set<size_t> sparseIdxs;
    calculate(points, sparseIdxs, 0, points.size() - 1);
    for (size_t idx : sparseIdxs) {
        sparsePoints.push_back(points[idx]);
    }
}


// 给定起点和终点，递归抽稀
bool calculate(const std::vector<Point>& points
                        , std::set<size_t> &sparseIdxs
                        , size_t start, size_t end) {
    if ((int32_t)start > (int32_t)end) {
        return false;
    }
    float maxL = 0;
    size_t peakIdx = start;
    sparseIdxs.insert(start);
    sparseIdxs.insert(end);
    for (size_t i = start + 1; i < end; i++) {
        int32_t d = calcDist(points[i], points[start], points[end]);
        float l = convertToRealDist(d, points[start], points[end]);
        if (l > maxL) {
            maxL = l;
            peakIdx = i;
        }
    }
    if (maxL > threshold) {
        calculate(points, sparseIdxs, start, peakIdx);
        calculate(points, sparseIdxs, peakIdx, end);
    }
    return true;
}

// 计算点到直线的距离
int32_t calcDist(const Point &p, const Point &start, const Point &end) {
    if (start.x == end.x && start.y == end.y) {
        // TRICKY: 无需开方
        int32_t d = (p.x - start.x) * (p.x - start.x) + (p.y - start.y) * (p.y - start.y);
        return d;
    }

    // 直线方程的系数 A, B, C
    int32_t A = end.y - start.y;
    int32_t B = start.x - end.x;
    int32_t C = end.x * start.y - start.x * end.y;

    // 使用点到直线的距离公式
    // TRICKY: 无需除以start到end的长度，节约计算量
    int32_t l = std::abs(A * p.x + B * p.y + C);
    return l;
}

// 转换为真实距离
float convertToRealDist(int32_t l, const Point &start, const Point &end) {
    if (start.x == end.x && start.y == end.y) {
        return std::sqrt(l);
    } else {
        int32_t A = end.y - start.y;
        int32_t B = start.x - end.x;
        return 1.0 * l / std::sqrt(A * A + B * B);
    }
}

};

// 测试函数
int main() {
    Sparser sparser;
    std::string output;
    sparser.sparse(input, output);
    std::cout << output << std::endl;
    return 0;
}
