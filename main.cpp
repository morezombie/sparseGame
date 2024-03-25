
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <regex>


constexpr float threshold = 6.0f;

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

void convertJsonToPoints(const std::string& json, std::vector<Point>& points) {
    // 输入形如
    /*
    [{
    x: 10,
    y: 60
}, {
    x: 106,
    y: 58
}]
*/
    // 使用正则表达式获取x和y的值
    std::regex re(R"((\d+),\s*(\d+))");
    std::smatch match;
    std::string::const_iterator begin = json.begin();
    std::string::const_iterator end = json.end();
    while (std::regex_search(begin, end, match, re)) {
        // 匹配成功，获取x和y的值
        int32_t x = std::stoi(match[1].str());
        int32_t y = std::stoi(match[2].str());
        // 将x和y添加到点的集合中
        Point p = {x, y};
        // 移动begin到下一次匹配的位置
        begin = match.suffix().first;
    }
}

void convertPointsToJson(const std::vector<Point>& points, std::string& json) {
    // 将points转换为字符串
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < points.size(); i++) {
        ss << "{" << "x:" << points[i].x << ", " << "y:" << points[i].y << "}";
        if (i < points.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    json = ss.str();
}

// 定义calculateSparsePoints函数
void calculateSparsePoints(const std::vector<Point>& points, std::vector<Point> &sparsePoints) {
    std::set<size_t> sparseIdxs;

    std::cout << "points size: " << points.size() << std::endl;
    calculate(points, sparseIdxs, 0, points.size() - 1);
    for (size_t idx : sparseIdxs) {
        sparsePoints.push_back(points[idx]);
    }
}


// 定义calculate函数
bool calculate(const std::vector<Point>& points
                        , std::set<size_t> &sparseIdxs
                        , size_t start, size_t end) {
    if ((int32_t)start >= (int32_t)end) {
        std::cout << "start: " << (int32_t)start << ", end: " << (int32_t)end << std::endl;
        return false;
    }
    float maxL = 0;
    sparseIdxs.insert(start);
    sparseIdxs.insert(end);
    for (size_t i = start; i < end; i++) {
        float l = calcDist(points[i], points[start], points[end]);
        maxL = std::max(maxL, l);
        std::cout << "maxL: " << maxL << std::endl;
        if (l > threshold) {
            calculate(points, sparseIdxs, start, i);
            calculate(points, sparseIdxs, i, end);
            break;
        }
    }
    return true;
}

float calcDist(const Point &p, const Point &start, const Point &end) {
    // 距离l = |(y2 - y1)*x3 + (x1 - x2)*y3 + (x2*y1 - x1*y2)| / sqrt((y2 - y2)^2 + (x2 - x2)^2) 
    float l = (end.y - start.y) * p.x + (start.x - end.x) * p.y + (start.y * end.x - end.y * start.x);
    l = std::abs(l) / sqrtf((end.y - start.y) * (end.y - start.y) + (start.x - end.x) * (start.x - end.x));
    std::cout << "l: " << l << std::endl;
    return l;
}

};


int main() {
    std::string input = R"(
const input = [{
    x: 10,
    y: 60
}, {
    x: 106,
    y: 58
}]
    )";
    Sparser sparser;
    std::string output;
    sparser.sparse(input, output);
    std::cout << output << std::endl;

    return 0;
}
