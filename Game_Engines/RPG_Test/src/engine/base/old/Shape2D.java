package engine.base.old;


import engine.base.*;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;

import java.util.LinkedList;
import java.util.Objects;
import java.util.logging.Level;

/**
 * 二维形状基类
 */
public abstract class Shape2D {
    private final ShapeType shapeType;// 图形类别
    private Point2D[] vertexList;// 顶点列表
    private Point2D[] showVertexList;// 顶点显示列表
    private Point2D size;// 长宽
    private Point2D showSize;// 显示长宽
    private Point2D arc;// 弧长宽
    private Point2D showArc;// 显示弧长宽
    private double radius;// 半径
    private double showRadius;// 显示半径
    private Point2D scalePoint;// 放缩向量
    private Point2D centerPoint;// 规定任何图形都有一个中心点。即为图形坐标。
    private Point2D showCenterPoint;// 中心点显示坐标
    private Point2D idleDirection;// 默认方向
    private Point2D showDirection;// 显示方向
    private Color strokeColor;// 描边颜色（设置为Color.TRANSPARENT则无描边）
    private Color fillColor;// 填充颜色（设置为Color.TRANSPARENT则无填充）
    private int lineWidth;// 描边宽度（设置为0则无描边）
    private boolean isOpaque;// 是否不透明（是否绘制）
    private final LinkedList<Shape2D> shape2DS; // 孩子形状列表
    private Shape2D parent; // 父形状引用
    private boolean isRotationBinding; // 是否旋转绑定
    private boolean isCenterSticking; // 是否中心粘连

    public Shape2D(ShapeType shapeType) {
        this.shapeType = shapeType;
        scalePoint = new Point2D(1.0, 1.0);
        centerPoint = new Point2D();
        idleDirection = new Point2D(1.0, 0);
        showDirection = idleDirection;
        isOpaque = true;
        strokeColor = Color.BLACK;
        fillColor = Color.WHITE;
        lineWidth = 1;
        shape2DS = new LinkedList<>();
        vertexList = new Point2D[0];
        showVertexList = new Point2D[0];
        isRotationBinding = false;
        isCenterSticking = false;
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("Shape2D(%d) CREATED.", this.hashCode());
        }
    }

    @Override
    public int hashCode() {
        return Objects.hash(shapeType, shape2DS);
    }

    /**
     * 设置父形状
     */
    public void setParent(Shape2D parent) {
        if (this.parent != null) {
            this.parent.untie(this);
            if (LogUtils.isLoggable(Level.FINER)){
                LogUtils.finer("Parent(%d) untied this(%d).",
                        this.parent.hashCode(), this.hashCode());
            }
        }
        this.parent = parent;
        if (this.parent != null) {
            this.parent.tie(this);
            if (LogUtils.isLoggable(Level.FINER)) {
                LogUtils.finer("A new parent(%d) tied this(%d).",
                        this.parent.hashCode(), this.hashCode());
            }
        }
        setCenterPoint(centerPoint);

    }

    /**
     * 添加孩子形状
     */
    void tie(Shape2D child) {
        shape2DS.add(child);
    }

    /**
     * 解除孩子形状绑定关系
     */
    void untie(Shape2D child) {
        shape2DS.remove(child);
    }

    public void setRotationBinding(boolean rotationBinding) {
        isRotationBinding = rotationBinding;
        if (LogUtils.isLoggable(Level.FINE)) {
            LogUtils.fine("%s(%d) set rotation binding: %b", shapeType, this.hashCode(), isRotationBinding);
        }
    }

    public boolean isRotationBinding() {
        return isRotationBinding;
    }

    public void setCenterSticking(boolean centerSticking) {
        isCenterSticking = centerSticking;
        LogUtils.fine("%s(%d) set center sticking: %b", shapeType, this.hashCode(), isRotationBinding);
    }

    public boolean isCenterSticking() {
        return isCenterSticking;
    }

    /**
     * 设置顶点列表并刷新显示顶点坐标列表
     */
    public void setVertexList(Point2D[] vertexList) {
        this.vertexList = vertexList;
        showVertexList = new Point2D[vertexList.length];
        flashShowVertexList();
    }
    public void setVertexListWithoutFlash(Point2D[] vertexList) {
        this.vertexList = vertexList;
        showVertexList = new Point2D[vertexList.length];
    }

    public Point2D[] getVertexList() {
        return vertexList;
    }

    /**
     * 设置长宽并刷新
     * @param size 长宽
     */
    public void setSize(Point2D size) {
        this.size = size;
        showSize = Point2D.scale(Point2D.scale(this.size, scalePoint), SceneMapManager.getCamera().getZoomFactor());
    }

    public Point2D getSize() {
        return size;
    }

    public Point2D getShowSize() {
        return showSize;
    }

    public void setArc(Point2D arc) {
        this.arc = arc;
        showArc = Point2D.scale(Point2D.scale(this.arc, scalePoint), SceneMapManager.getCamera().getZoomFactor());
    }

    public Point2D getArc() {
        return arc;
    }

    public Point2D getShowArc() {
        return showArc;
    }

    /**
     * 设置圆的半径并刷新显示半径。
     * <p>注：规定对圆而言，放缩的倍数只看scalePoint的x分量</p>
     * @param radius 设置的半径
     */
    public void setRadius(double radius) {
        this.radius = radius;
        showRadius = radius * scalePoint.x * SceneMapManager.getCamera().getZoomFactor();
    }

    public double getRadius() {
        return radius;
    }

    public double getShowRadius() {
        return showRadius;
    }

    public ShapeType getShapeType() {
        return shapeType;
    }

    /**
     * 安全地设置中心点并刷新显示中心点和显示顶点列表。
     * <p>递归刷新孩子显示中心点和显示顶点列表</p>
     */
    public void setCenterPoint(Point2D centerPoint) {
        this.centerPoint = centerPoint;
        flashShowCenterPoint();
        flashShowVertexList();
        for (Shape2D child : shape2DS) {
            child.flashShowCenterPoint();
            child.flashShowVertexList();
        }
    }
    public void setCenterPointWithoutFlash(Point2D centerPoint) {
        this.centerPoint = centerPoint;
    }

    public Point2D getCenterPointRelative() {
        return centerPoint;
    }

    public Point2D getCenterPointAbsolute() {
        Shape2D par = parent;
        Point2D centerPointAbs = new Point2D(centerPoint);
        while (par != null) {
            centerPointAbs.add(par.getCenterPointRelative());
            par = par.getParent();
        }
        return centerPointAbs;
    }

    public Point2D getCenterPoint() {
        return centerPoint;
    }

    public Point2D getShowCenterPoint() {
        return showCenterPoint;
    }

    public Shape2D getParent() {
        return parent;
    }

    public void setIdleDirection(Point2D idleDirection) {
        this.idleDirection = idleDirection;
    }

    public Point2D getIdleDirection() {
        return idleDirection;
    }

    public Point2D getShowDirection() {
        return showDirection;
    }

    public void setFillColor(Color fillColor) {
        this.fillColor = fillColor;
    }

    public Color getFillColor() {
        return fillColor;
    }

    public void setStrokeColor(Color strokeColor) {
        this.strokeColor = strokeColor;
    }

    public Color getStrokeColor() {
        return strokeColor;
    }

    public void setLineWidth(int lineWidth) {
        this.lineWidth = lineWidth;
    }

    public int getLineWidth() {
        return lineWidth;
    }

    public void setOpaque(boolean opaque) {
        isOpaque = opaque;
    }

    public boolean isOpaque() {
        return isOpaque;
    }

    /**
     * 安全地设置放缩向量。并刷新显示大小。
     * <p>对 <0 的分量取 0；</p>
     * <p>对 圆形放缩向量仅x分量生效</p>
     * @param scalePoint 放缩向量
     */
    public void setScalePoint(Point2D scalePoint) {
        this.scalePoint = scalePoint;
        if (scalePoint.x < 0)
            this.scalePoint.x = 0;
        if (scalePoint.y < 0)
            this.scalePoint.y = 0;

        flashShowSizeParams();
        flashShowVertexList();
    }
    public void setScalePointWithoutFlash(Point2D scalePoint) {
        this.scalePoint = scalePoint;
        if (scalePoint.x < 0)
            this.scalePoint.x = 0;
        if (scalePoint.y < 0)
            this.scalePoint.y = 0;
    }

    public Point2D getScalePoint() {
        return scalePoint;
    }

    /**
     * 刷新尺寸显示参数
     * <p>对于三角形、梯形、多边形不起作用</p>
     */
    public void flashShowSizeParams() {
        Camera camera = SceneMapManager.getCamera();
        // 刷新显示大小
        switch (shapeType) {

            case Circle:
                showRadius = radius * scalePoint.x * camera.getZoomFactor();
                break;
            case Oval:
            case Rect:
                showSize = Point2D.scale(Point2D.scale(this.size, scalePoint), camera.getZoomFactor());
                break;
            case RoundRect:
                showSize = Point2D.scale(Point2D.scale(this.size, scalePoint), camera.getZoomFactor());
                showArc = Point2D.scale(Point2D.scale(this.arc, scalePoint), camera.getZoomFactor());
                break;
        }
    }

    /**
     * 刷新顶点显示坐标
     */
    public void flashShowVertexList() {
        for (int i = 0; i < showVertexList.length; i++) {
            showVertexList[i] = SceneMapManager.getCamera().worldToCamera(
                    Point2D.plus(getCenterPointAbsolute(), Point2D.scale(vertexList[i], scalePoint))
            );
        }
    }

    /**
     * 刷新中心点显示坐标
     */
    public void flashShowCenterPoint() {
        showCenterPoint = SceneMapManager.getCamera().worldToCamera(getCenterPointAbsolute());
    }

    /**
     * 以位移改变图形坐标并刷新中心点和顶点显示坐标。
     * <p>递归刷新孩子中心点和顶点显示坐标</p>
     * @param displacement 位移
     */
    public void move(Point2D displacement) {
        setCenterPoint(Point2D.plus(centerPoint, displacement));
    }
    public void moveWithoutFlash(Point2D displacement) {
        centerPoint.add(displacement);
    }

    /**
     * 围绕中心点旋转改变顶点坐标并刷新各顶点显示坐标。
     * <p>递归作用于孩子（对所有孩子的标签分支）</p>
     * @param radian 弧度
     */
    public void rotate(double radian) {
        switch (shapeType) {
            case Triangle:
            case Trapezoid:
            case Polygon:
                rotateVerticesWithoutFlash(radian);
                flashShowVertexList();
                break;
        }
        rotateRecursively(radian);
    }
    public void rotateWithoutFlash(double radian) {
        switch (shapeType) {
            case Triangle:
            case Trapezoid:
            case Polygon:
                rotateVerticesWithoutFlash(radian);
                break;
        }
        rotateRecursivelyWithoutFlash(radian);
    }

    /**
     * 旋转各顶点坐标（无刷新）
     */
    private void rotateVerticesWithoutFlash(double radian) {
        for (Point2D vertex : vertexList) {
            vertex.rotate(radian);
        }
    }

    private void rotateRecursively(double radian) {
        boolean shouldBeFlashed = false;
        for (Shape2D child : shape2DS) {
            if (child.isCenterSticking) {
                child.centerPoint.rotate(radian);
                child.flashShowCenterPoint();
                shouldBeFlashed = true;
            }
            if (child.isRotationBinding) {
                child.rotateVerticesWithoutFlash(radian);
                shouldBeFlashed = true;
            }
            if (shouldBeFlashed) child.flashShowVertexList();
            shouldBeFlashed = false;
            child.rotateRecursively(radian);
        }
    }
    private void rotateRecursivelyWithoutFlash(double radian) {
        for (Shape2D child : shape2DS) {
            if (child.isCenterSticking) {
                child.centerPoint.rotate(radian);
            }
            if (child.isRotationBinding) {
                child.rotateVerticesWithoutFlash(radian);
            }
            child.rotateRecursivelyWithoutFlash(radian);
        }
    }

    /**
     * 绘制
     */
    public final void Draw(GraphicsContext gc) {
        if (isOpaque) {
            gc.setFill(fillColor);
            gc.setStroke(strokeColor);
            gc.setLineWidth(lineWidth);
            DrawBefore();
            switch (shapeType) {

                case Circle:
                    gc.fillOval(showVertexList[0].x, showVertexList[0].y, showRadius, showRadius);
                    gc.strokeOval(showVertexList[0].x, showVertexList[0].y, showRadius, showRadius);
                    break;
                case Oval:
                    gc.fillOval(showVertexList[0].x, showVertexList[0].y, showSize.x, showSize.y);
                    gc.strokeOval(showVertexList[0].x, showVertexList[0].y, showSize.x, showSize.y);
                    break;
                case Rect:
                    gc.fillRect(showVertexList[0].x, showVertexList[0].y, showSize.x, showSize.y);
                    gc.strokeRect(showVertexList[0].x, showVertexList[0].y, showSize.x, showSize.y);
                    break;
                case RoundRect:
                    gc.fillRoundRect(showVertexList[0].x, showVertexList[0].y,
                            showSize.x, showSize.y, showArc.x, showArc.y);
                    gc.strokeRoundRect(showVertexList[0].x, showVertexList[0].y,
                            showSize.x, showSize.y, showArc.x, showArc.y);
                    break;
                case Triangle:
                case Trapezoid:
                case Polygon:
                    int nPoints = showVertexList.length;
                    double[] xPoints = new double[nPoints];
                    double[] yPoints = new double[nPoints];
                    for (int i = 0; i < showVertexList.length; i++) {
                        xPoints[i] = showVertexList[i].x;
                        yPoints[i] = showVertexList[i].y;
                    }
                    gc.fillPolygon(xPoints, yPoints, nPoints);
                    gc.strokePolygon(xPoints, yPoints, nPoints);
                    break;
            }
            DrawAfter();
        }
    }

    /**
     * 自定义前绘制函数（选择重写，仅不透明时触发）
     */
    public void DrawBefore() {

    }

    /**
     * 自定义后绘制函数（选择重写，仅不透明时触发）
     */
    public void DrawAfter() {

    }

    /**
     * （测试）顶点列表字符串形式
     */
    public String vertexListToString() {
        String str = "";
        for (Point2D vertex : vertexList) {
            str = str.concat(vertex.toString());
            str = str.concat("->norm: " + vertex.norm() + ", ");
        }
        return str;
    }


}
