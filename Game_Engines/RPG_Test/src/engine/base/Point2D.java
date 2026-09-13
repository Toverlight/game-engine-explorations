package engine.base;

/**
 * 二维点。双精度存储
 */
public class Point2D {
    public double x;
    public double y;

    /**
     * 无参数。默认初始化为0
     */
    public Point2D() {
        x = 0;
        y = 0;
    }

    /**
     * 以给定数据初始化一个点
     */
    public Point2D(double x, double y) {
        this.x = x;
        this.y = y;
    }

    /**
     * 拷贝构造
     * @param point2D 要拷贝的点
     */
    public Point2D(Point2D point2D) {
        x = point2D.x;
        y = point2D.y;
    }

    /**
     * 用已有的点赋值
     */
    public void assign(Point2D point2D) {
        x = point2D.x;
        y = point2D.y;
    }

    /**
     * 直接加上向量
     */
    public void add(Point2D other) {
        x += other.x;
        y += other.y;
    }

    /**
     * 将两个向量相加并返回结果
     */
    public static Point2D plus(Point2D a, Point2D b) {
        return new Point2D(a.x + b.x, a.y + b.y);
    }

    /**
     * 求若干个向量的和并返回结果
     */
    public static Point2D plusAll(Point2D... point2DS) {
        Point2D point2D = new Point2D();
        for (Point2D p2 : point2DS) {
            point2D.add(p2);
        }
        return point2D;
    }

    /**
     * 直接取反
     */
    public void invert() {
        x = -x;
        y = -y;
    }

    /**
     * 取反一个向量并返回结果
     */
    public static Point2D invert(Point2D point2D) {
        return new Point2D(-point2D.x, -point2D.y);
    }

    /**
     * 直接减去向量
     */
    public void sub(Point2D other) {
        x -= other.x;
        y -= other.y;
    }

    /**
     * 将两个向量相减并返回结果
     */
    public static Point2D minus(Point2D a, Point2D b) {
        return new Point2D(a.x - b.x, a.y - b.y);
    }

    /**
     * 点乘另一个向量并返回结果
     */
    public double dot(Point2D other) {
        return x * other.x + y * other.y;
    }

    /**
     * 求两个向量的内积并返回结果
     */
    public static double product(Point2D a, Point2D b) {
        return a.x * b.x + a.y * b.y;
    }

    /**
     * 求两个向量张成的有向面积（叉乘）
     */
    public double cross(Point2D other) {
        return x * other.y - other.x * y;
    }

    /**
     * 平方。求一个数的平方并返回结果
     */
    public static double square(double num) {
        return num * num;
    }

    /**
     * 求与另一个向量的距离并返回结果
     */
    public double dis(Point2D other) {
        if (x == other.x && y == other.y) return 0;
        return Math.sqrt(square(x - other.x) + square(y - other.y));
    }

    /**
     * 求两个向量的欧几里得距离并返回结果
     */
    public static double distance(Point2D a, Point2D b) {
        return Math.sqrt(square(a.x - b.x) + square(a.y - b.y));
    }

    /**
     * 求与另一个向量的切比雪夫距离并返回结果
     */
    public double chebyshevDis(Point2D other) {
        return Math.max(Math.abs(x - other.x), Math.abs(y - other.y));
    }

    /**
     * 求两个向量的切比雪夫距离并返回结果
     */
    public static double chebyshevDistance(Point2D a, Point2D b) {
        return Math.max(Math.abs(a.x - b.x), Math.abs(a.y - b.y));
    }

    /**
     * 求与另一个向量的曼哈顿距离并返回结果
     */
    public double manhattanDis(Point2D other) {
        return Math.abs(x - other.x) + Math.abs(y - other.y);
    }

    /**
     * 求两个向量的曼哈顿距离并返回结果
     */
    public static double manhattanDistance(Point2D a, Point2D b) {
        return Math.abs(a.x - b.x) + Math.abs(a.y - b.y);
    }

    /**
     * 创建一个向量旋转矩阵
     */
    private static double[][] createVecRotateMatrix(double radian) {
        double cos = Math.cos(radian);
        double sin = Math.sin(radian);
        return new double[][]{
                {cos, -sin},
                {sin, cos}
        };
    }

    /**
     * 创建一个坐标系旋转矩阵
     */
    private static double[][] createCorSysRotateMatrix(double radian) {
        double cos = Math.cos(radian);
        double sin = Math.sin(radian);
        return new double[][]{
                {cos, sin},
                {-sin, cos}
        };
    }

    /**
     * 直接旋转一定弧度
     * 窗口显示下顺时针为正
     * @param radian 弧度
     */
    public void rotate(double radian) {
//        double n = norm();
//        if (n == 0) return;
        if (x == 0 && y == 0) return;
        double[][] rotateMatrix = createVecRotateMatrix(radian);
        x = rotateMatrix[0][0] * x + rotateMatrix[0][1] * y;
        y = rotateMatrix[1][0] * x + rotateMatrix[1][1] * y;
//        double biasedN = norm();
//        if (biasedN == 0) return;
//        scale(n / biasedN);
    }

    /**
     * 将一个向量旋转一定弧度并返回结果
     * 窗口显示下顺时针为正
     * @param radian 弧度
     */
    public static Point2D rotate(Point2D point2D, double radian) {
//        double n = point2D.norm();
//        if (n == 0) return new Point2D();
        if (point2D.x == 0 && point2D.y == 0) return new Point2D();
        double[][] rotateMatrix = createVecRotateMatrix(radian);
        //        double biasedN = resPoint.norm();
//        if (biasedN == 0) return new Point2D();
//        return Point2D.scale(resPoint, n / biasedN);
        return new Point2D(
                rotateMatrix[0][0] * point2D.x + rotateMatrix[0][1] * point2D.y,
                rotateMatrix[1][0] * point2D.x + rotateMatrix[1][1] * point2D.y
        );
    }

    /**
     * 获取当前向量在旋转坐标系后的新表示
     * 窗口显示下顺时针为正
     * @param radian 弧度
     */
    public Point2D getExprInRotatedCorSys(double radian) {
        double[][] rotateMatrix = createCorSysRotateMatrix(radian);
        return new Point2D(
            rotateMatrix[0][0] * x + rotateMatrix[0][1] * y,
            rotateMatrix[1][0] * x + rotateMatrix[1][1] * y
        );
    }

    /**
     * 求向量的模
     */
    public double norm() {
        return Math.sqrt(x * x + y * y);
    }

    /**
     * 直接等比放缩
     * @param factor 倍数
     */
    public void scale(double factor) {
        x *= factor;
        y *= factor;
    }
    /**
     * 直接不等比放缩
     * @param scalePoint 放缩向量
     */
    public void scale(Point2D scalePoint) {
        x *= scalePoint.x;
        y *= scalePoint.y;
    }

    /**
     * 创建一个向量等比放缩一定倍数的副本并返回结果
     * @param factor 倍数
     */
    public static Point2D scale(Point2D point2D, double factor) {
        return new Point2D(point2D.x * factor, point2D.y * factor);
    }
    /**
     * 创建一个向量不等比放缩的副本并返回结果
     * @param scalePoint 放缩向量
     */
    public static Point2D scale(Point2D point2D, Point2D scalePoint) {
        return new Point2D(point2D.x * scalePoint.x, point2D.y * scalePoint.y);
    }

    /**
     * 直接标准化
     * 若是零向量则不改变
     */
    public void normalize() {
        double norm = norm();
        if (norm != 0) {
            x /= norm;
            y /= norm;
        }
    }

    /**
     * 获取一个向量的标准化向量
     * 若是零向量则返回零向量
     */
    public static Point2D normalize(Point2D point2D) {
        double norm = point2D.norm();
        if (norm != 0) {
            return new Point2D(point2D.x / norm, point2D.y / norm);
        }
        return new Point2D();
    }

    /**
     * 求该向量与另一个向量的夹角并返回结果
     * 若其中一个为零向量，则返回 0
     */
    public double includedAngle(Point2D point2D) {
        double normA = norm();
        double normB = point2D.norm();
        if (normA == 0 || normB == 0) return 0; // 零向量与任何向量平行
        return Math.acos(dot(point2D) / (normA * normB));
    }

    /**
     * 将角度转换为弧度
     * @param angle 角度
     */
    public static double convert2Radian(double angle) {
        return angle * Math.PI / 180.0;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("(");
        if (Math.abs(x) < 1e-10) {
            sb.append("0.0");
        } else {
            sb.append(String.format("%.10f", x));
        }
        sb.append(", ");
        if (Math.abs(y) < 1e-10) {
            sb.append("0.0");
        } else {
            sb.append(String.format("%.10f", y));
        }
        sb.append(")");
        return sb.toString();
    }
}
