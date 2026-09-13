package engine.base.calc;

import engine.base.Point2D;

/**
 * 矩阵。以double存储
 */
public class Matrix {
    private final double[][] data;
    private final int rows;
    private final int cols;

    /**
     * 创建二维矩阵，默认值全0
     * @param rows 行数
     * @param cols 列数
     */
    public Matrix(int rows, int cols) {
        this.rows = rows;
        this.cols = cols;
        this.data = new double[rows][cols];
    }

    /**
     * 创建二维矩阵，默认值指定
     * @param rows 行数
     * @param cols 列数
     * @param initialValue 默认值
     */
    public Matrix(int rows, int cols, int initialValue) {
        this.rows = rows;
        this.cols = cols;
        this.data = new double[rows][cols];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                this.data[i][j] = initialValue;
            }
        }
    }

    /**
     * 用二维数组初始化一个矩阵
     * @param data 二维数组
     */
    public Matrix(double[][] data) {
        this.rows = data.length;
        this.cols = data[0].length;
        this.data = data;
    }

    /**
     * 获取元素
     * @param row 行号
     * @param col 列号
     */
    public double get(int row, int col) {
        return data[row][col];
    }

    /**
     * 为元素赋值
     * @param row 行号
     * @param col 列号
     * @param value 值
     */
    public void set(int row, int col, double value) {
        data[row][col] = value;
    }

    /**
     * 获取行数（第一维度）
     * @return 行数
     */
    public int getRows() {
        return rows;
    }

    /**
     * 获取列数（第二维度）
     * @return 列数
     */
    public int getCols() {
        return cols;
    }

    /**
     * 加法
     */
    public Matrix add(Matrix other) {
        if (this.rows != other.rows || this.cols != other.cols) {
            throw new IllegalArgumentException("Matrix dimensions must match for addition!");
        }
        Matrix result = new Matrix(this.rows, this.cols);
        for (int i = 0; i < this.rows; i++) {
            for (int j = 0; j < this.cols; j++) {
                result.data[i][j] = this.data[i][j] + other.data[i][j];
            }
        }
        return result;
    }

    /**
     * 数加
     */
    public Matrix add(double value) {
        Matrix result = new Matrix(this.rows, this.cols);
        for (int i = 0; i < this.rows; i++) {
            for (int j = 0; j < this.cols; j++) {
                result.data[i][j] = this.data[i][j] + value;
            }
        }
        return result;
    }

    /**
     * 乘法
     */
    public Matrix multiply(Matrix other) {
        if (this.cols != other.rows) {
            throw new IllegalArgumentException("Matrix dimensions must match for multiplication!");
        }
        Matrix result = new Matrix(this.rows, other.cols);
        for (int i = 0; i < this.rows; i++) {
            for (int j = 0; j < other.cols; j++) {
                for (int k = 0; k < this.cols; k++) {
                    result.data[i][j] += this.data[i][k] * other.data[k][j];
                }
            }
        }
        return result;
    }

    /**
     * 数乘
     */
    public Matrix multiply(double value) {
        Matrix result = new Matrix(this.rows, this.cols);
        for (int i = 0; i < this.rows; i++) {
            for (int j = 0; j < this.cols; j++) {
                result.data[i][j] = this.data[i][j] * value;
            }
        }
        return result;
    }

    /**
     * 转置
     */
    public Matrix transpose() {
        Matrix result = new Matrix(this.cols, this.rows);
        for (int i = 0; i < this.rows; i++) {
            for (int j = 0; j < this.cols; j++) {
                result.data[j][i] = this.data[i][j];
            }
        }
        return result;
    }

    /**
     * 仿射变换
     * @param point2D 二维点
     */
    public Point2D transform(Point2D point2D) {
        if (this.cols != 3 || this.rows != 3) {
            throw new IllegalArgumentException("Matrix dimensions must match for Point2D!Maybe you should " +
                    "initialize a affine transform matrix(by <init>).");
        }
        Point2D resultPoint = new Point2D();
        Matrix extendedPoint = new Matrix(3,1);
        extendedPoint.data[0][0] = point2D.x;
        extendedPoint.data[1][0] = point2D.y;
        extendedPoint.data[2][0] = 1.0;
        Matrix result = this.multiply(extendedPoint);
        resultPoint.x = result.data[0][0];
        resultPoint.y = result.data[1][0];
        return resultPoint;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        boolean[] needToAlign = new boolean[cols];

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                double value = data[i][j];

                if (!(Math.abs(value) < 1e-10)) {
                    needToAlign[j] = true;
                }
            }
        }

        for (int i = 0; i < rows; i++) {
            boolean isThisLong = false;
            // 遍历每一行，添加[和行数据
            sb.append('[');

            // 遍历该行的每个元素
            for (int j = 0; j < cols; j++) {
                double value = data[i][j];

                // 检查是否为零，并格式化为0.0
                if (Math.abs(value) < 1e-10) {
                    sb.append("0.0");
                    isThisLong = false;
                } else {
                    // 格式化到小数点后十位
                    sb.append(String.format("%.10f", value));
                    isThisLong = true;
                }

                // 添加列对齐的空格（根据需要调整）
                if (j < cols - 1) {
                    if (needToAlign[j] && !isThisLong) {
                        sb.append("          ");
                    } else {
                        sb.append(" ");
                    }
                }
            }

            // 结束当前行，并添加]和换行
            sb.append(']');
            sb.append("\n");
        }

        return sb.toString();
    }

    /**
     * 创建一个旋转矩阵
     * @param theta 弧度
     */
    public static Matrix rotateMatrix(double theta) {
        double cos = Math.cos(theta);
        double sin = Math.sin(theta);
        double[][] data = {
                {cos, -sin, 0},
                {sin, cos, 0},
                {0, 0, 1}
        };
        return new Matrix(data);
    }

    /**
     * 创建一个平移矩阵
     * @param tx x方向平移
     * @param ty y方向平移
     */
    public static Matrix translateMatrix(double tx, double ty) {
        double[][] data = {
                {1, 0, tx},
                {0, 1, ty},
                {0, 0, 1}
        };
        return new Matrix(data);
    }

    /**
     * 创建一个等比缩放矩阵
     * @param s 缩放比例
     */
    public static Matrix scaleMatrix(double s) {
        double[][] data = {
                {s, 0, 0},
                {0, s, 0},
                {0, 0, 1}
        };
        return new Matrix(data);
    }

    /**
     * 创建一个不等比缩放矩阵
     * @param sx x方向缩放比例
     * @param sy y方向缩放比例
     */
    public static Matrix scaleMatrix(double sx, double sy) {
        double[][] data = {
                {sx, 0, 0},
                {0, sy, 0},
                {0, 0, 1}
        };
        return new Matrix(data);
    }


    public static void main(String[] args) {
        Matrix transformMatrix = Matrix.translateMatrix(1, 1)
                .multiply(Matrix.rotateMatrix(Math.PI / 2));
        Point2D aPoint = new Point2D(1,0);
        System.out.println(transformMatrix.transform(aPoint));
    }
}
