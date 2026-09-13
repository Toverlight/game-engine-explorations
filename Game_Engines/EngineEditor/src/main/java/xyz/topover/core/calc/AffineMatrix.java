package xyz.topover.core.calc;

/**
 * <h1>仿射变换矩阵</h1>
 * <p>具有旋转、平移、缩放、剪切功能。</p>
 * <p>设置完后，请使用<b>transform方法作用于Point2D点</b>。</p>
 */
public class AffineMatrix {
    /**
     * data 有6个元素，0 1 3 4位的为线性因子，2 5位的为平移因子。
     */
    private double[] data;

    /**
     * <h1>2D点的仿射（变换）矩阵</h1>
     * <p>初始化为<b>2×3矩阵：</b></p>
     * <p>[[<b>1</b>, 0, 0],<br>
     *  &nbsp;[0, <b>1</b>, 0]]</p>
     */
    public AffineMatrix() {
        data = new double[6];
        // 初始化为单位矩阵
        data[0] = 1;
        data[4] = 1;
    }

    /**
     * <h1>旋转变换</h1>
     * <p>规定<b>顺时针为正</b>。</p>
     * @param radian 弧度
     */
    public void rotate(double radian) {
        AffineMatrix rotateMatrix = new AffineMatrix();
        double cos = Math.cos(radian);
        double sin = Math.sin(radian);

        rotateMatrix.data[0] = cos;
        rotateMatrix.data[1] = -sin;
        rotateMatrix.data[3] = sin;
        rotateMatrix.data[4] = cos;

        data = rotateMatrix.multiply(this);
    }
    /**
     * <h1>平移变换</h1>
     * @param tx x方向平移
     * @param ty y方向平移
     */
    public void translate(double tx, double ty) {
        data[2] += tx;
        data[5] += ty;
    }

    /**
     * <h1>缩放变换</h1>
     * @param sx x方向缩放倍数
     * @param sy y方向缩放倍数
     */
    public void scale(double sx, double sy) {
        AffineMatrix scaleMatrix = new AffineMatrix();

        scaleMatrix.data[0] = sx;
        scaleMatrix.data[4] = sy;

        data = scaleMatrix.multiply(this);
    }

    /**
     * <h1>等比例缩放变换</h1>
     * <p>变换后<b>形状不变</b>。</p>
     * @param s 比例因子
     */
    public void scale(double s) {
        AffineMatrix scaleMatrix = new AffineMatrix();

        scaleMatrix.data[0] = s;
        scaleMatrix.data[4] = s;

        data = scaleMatrix.multiply(this);
    }

    /**
     * <h1>x方向剪切变换</h1>
     * <p>变换后<b>面积不变</b>。</p>
     * @param hx x方向剪切因子
     */
    public void shearX(double hx) {
        AffineMatrix shearXMatrix = new AffineMatrix();

        shearXMatrix.data[1] = hx;

        data = shearXMatrix.multiply(this);
    }
    /**
     * <h1>y方向剪切变换</h1>
     * <p>变换后<b>面积不变</b>。</p>
     * @param hy y方向剪切因子
     */
    public void shearY(double hy) {
        AffineMatrix shearYMatrix = new AffineMatrix();

        shearYMatrix.data[3] = hy;

        data = shearYMatrix.multiply(this);
    }

    /**
     * 乘
     */
    private double[] multiply(AffineMatrix other) {
        double[] resData = new double[6];
        resData[0] = data[0] * other.data[0] + data[1] * other.data[3];
        resData[1] = data[0] * other.data[1] + data[1] * other.data[4];
        resData[2] = data[0] * other.data[2] + data[1] * other.data[5] + data[2];
        resData[3] = data[3] * other.data[0] + data[4] * other.data[3];
        resData[4] = data[3] * other.data[1] + data[4] * other.data[4];
        resData[5] = data[3] * other.data[2] + data[4] * other.data[5] + data[5];
        return resData;
    }

    /**
     * <h1>变换点坐标</h1>
     * <p>P' = M·P</p>
     * @param point2D P
     * @return 变换后 P'
     */
    public Point2D transform(Point2D point2D) {
        Point2D resultPoint = new Point2D();
        resultPoint.x = data[0] * point2D.x + data[1] * point2D.y + data[2];
        resultPoint.y = data[3] * point2D.x + data[4] * point2D.y + data[5];
        return resultPoint;
    }

    /**
     * <h1>获取指定位置的元素</h1>
     * @param row 行索引
     * @param col 列索引
     * @return 元素值
     */
    public double get(int row, int col) {
        return data[row * 3 + col];
    }


    public static void main(String[] args) {
        AffineMatrix matrix = new AffineMatrix();

        matrix.translate(3, 4);
        matrix.rotate(Math.PI / 2);
        matrix.scale(2, 1);

        Point2D oldP = new Point2D();
        Point2D newP = matrix.transform(oldP);

        System.out.println("new P: " + newP);
    }
}
