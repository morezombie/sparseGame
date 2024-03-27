# RUN whith python3!!!!!
import math

# input here boss :)
threshold = 6;
input = [{
    'x': 10,
    'y': 60
}, {
    'x': 12,
    'y': 46
}]

def main():
    sparser = Sparser()
    print(sparser.sparse(input, threshold))

class Sparser:
    def calcDist(self, p, start, end):
        if (start['x'] == end['x'] and start['y'] == end['y']):
            d = (p['x'] - start['x']) * (p['x'] - start['x']) + (p['y'] - start['y']) * (p['y'] - start['y'])
            return d
        
        A = end['y'] - start['y']
        B = start['x'] - end['x']
        C = end['x'] * start['y'] - start['x'] * end['y']
        
        l = abs(A * p['x'] + B * p['y'] + C)
        return l

    def convertToRealDist(self, l, start, end):
        if (start['x'] == end['x'] and start['y'] == end['y']):
            return math.sqrt(l)
        A = end['y'] - start['y']
        B = start['x'] - end['x']
        return 1.0 * l / math.sqrt(A * A + B * B)

    def calculate(self, points, threshold, sparseIdxs, start, end):
        if (start > end):
            return False
        maxL = 0
        peakIdx = start
        sparseIdxs.add(start)
        sparseIdxs.add(end)
        for i in range(start + 1, end):
            d = self.calcDist(points[i], points[start], points[end])
            l = self.convertToRealDist(d, points[start], points[end])
            if l > maxL:
                maxL = l
                peakIdx = i
        if maxL > threshold:
            self.calculate(points, threshold, sparseIdxs, start, peakIdx)
            self.calculate(points, threshold, sparseIdxs, peakIdx, end)
        return True

    def sparse(self, points, threshold):
        sparseIdxs = set()
        self.calculate(points, threshold, sparseIdxs, 0, len(points) - 1)
        sparseIdxs = sorted(sparseIdxs)
        return [points[i] for i in sparseIdxs]

if __name__ == '__main__':
    main()
