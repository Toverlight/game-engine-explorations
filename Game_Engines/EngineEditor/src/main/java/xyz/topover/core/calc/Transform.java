package xyz.topover.core.calc;

/**
 * <h1>变换</h1>
 * <p>包含相对坐标、旋转弧度、缩放、父变换、相对到世界（和世界到相对，逆矩阵）仿射变换矩阵、脏（与逆脏）标记、跟随类型、变换偏置属性。</p>
 * <p>
 *    <b>· 相对坐标</b>：相对于父变换的坐标。顶层时即为世界坐标。<br>
 *    <b>· 旋转弧度</b>：窗口系下，顺时针旋转的弧度。<br>
 *    <b>· 缩放</b>：x和y方向的缩放因子。<br>
 *    <b>· 剪切</b>：x和y方向的剪切因子。<br>
 *    <b>· 父变换</b>：绑定的父变换。<br>
 *    <b>· 相对到世界仿射变换矩阵</b>：将相对坐标映射到世界坐标的变换矩阵。<br>
 *    <b>· 世界到相对仿射变换矩阵</b>：将世界坐标映射到相对坐标的变换矩阵。<br>
 *    <b>· （逆）脏标记</b>：任何修改都会将该变换标记为“（逆）脏”，（逆）脏的变换将在获取相对到世界（世界到相对）仿射变换矩阵
 *    返回前更新变换矩阵，并擦除“（逆）脏”。<br>
 *    <b>· 跟随类型</b>：这个值决定该变换跟随父变换的行为。<br>
 *    <b>· 变换偏置</b>：这个值决定线性变换的基准点。默认原点。
 * </p>
 */
public class Transform {
    /**
     * <h1>跟随类型</h1>
     * <p>这个值决定该变换跟随父变换的行为，有四种值：<br>
     *    <b>· 仅平移</b>(T)<br>
     *    <b>· 旋转和平移</b>(RT)<br>
     *    <b>· 缩放和平移</b>(ST)<br>
     *    <b>· 旋转、缩放和平移</b>(RST)
     * </p>
     */
    public enum I {
        /**
         * <h1>仅平移</h1>
         */
        T,
        /**
         * <h1>旋转和平移</h1>
         */
        RT,
        /**
         * <h1>缩放和平移</h1>
         */
        ST,
        /**
         * <h1>旋转、缩放和平移</h1>
         */
        RST
    }
    private final Point2D position; // 相对坐标
    private double rotation; // 窗口系下，顺时针旋转的弧度
    private final Point2D scale; // 缩放
    private final Point2D shear; // 剪切
    private Transform parent; // 父变换

    private AffineMatrix localToWorldMatrix; // 相对到世界仿射变换矩阵
    private AffineMatrix worldToLocalMatrix; // 世界到相对仿射变换矩阵
    private boolean isDirty = true; // 脏标记
    private boolean isInverseDirty = true; // 逆矩阵的脏标记

    private I i; // 跟随类型，决定该变换如何跟随父变换

    private final Point2D biasPoint;

    public Transform(I i) {
        position = new Point2D();
        rotation = 0;
        scale = new Point2D(1, 1);
        shear = new Point2D();
        parent = null;
        localToWorldMatrix = new AffineMatrix();
        worldToLocalMatrix = new AffineMatrix();
        this.i = i;
        biasPoint = new Point2D();
    }

    // ---- setter和getter ---- //
    public void setPosition(double x, double y) {
        position.x = x;
        position.y = y;
        markDirty();
        markInverseDirty();
    }

    public Point2D getPosition() {
        return new Point2D(position);
    }
    public Point2D getAbsPosition() {
        Transform t = this.parent;
        Point2D absPos = new Point2D(position);
        while (t != null) {
            absPos.add(t.position);
            t = t.parent;
        }
        return absPos;
    }

    public void setRotation(double rotation) {
        this.rotation = rotation;
        markDirty();
        markInverseDirty();
    }

    public double getRotation() {
        return rotation;
    }

    public void setScale(double sx, double sy) {
        scale.x = sx;
        scale.y = sy;
        markDirty();
        markInverseDirty();
    }
    public void setScale(double s) {
        scale.x = s;
        scale.y = s;
        markDirty();
        markInverseDirty();
    }

    public Point2D getScale() {
        return new Point2D(scale);
    }

    public void setShearX(double hx) {
        shear.x = hx;
        markDirty();
        markInverseDirty();
    }
    public void setShearY(double hy) {
        shear.y = hy;
        markDirty();
        markInverseDirty();
    }

    public void setParent(Transform parent) {
        this.parent = parent;
        markDirty();
        markInverseDirty();
    }

    public Transform getParent() {
        return parent;
    }

    public void setI(I i) {
        this.i = i;
        markDirty();
        markInverseDirty();
    }

    /**
     * <h1>设置变换偏置</h1>
     * <p>
     *     变换偏置<b>不遵循跟随原则</b>，仅最上层（即当前的变换）的有效。
     * </p>
     * @param biasPoint 变换偏置
     */
    public void setBiasPoint(Point2D biasPoint) {
        this.biasPoint.x = biasPoint.x;
        this.biasPoint.y = biasPoint.y;
        markDirty();
        markInverseDirty();
    }

    // ---- 自定义方法 ---- //
    /**
     * 标记需要更新矩阵
     */
    private void markDirty() {
        isDirty = true;
    }
    /**
     * 标记需要更新逆矩阵
     */
    private void markInverseDirty() {
        isInverseDirty = true;
    }

    /**
     * 更新矩阵
     */
    private void updateMatrix() {
        Point2D s = new Point2D(scale);
        Point2D pos = new Point2D(position);

        double r = totalEffect(pos, rotation, s);

        AffineMatrix result = new AffineMatrix();

        // 变换次序：Tb^-1 -> S -> R -> HX -> HY -> T·Tb (T须最后)
        if (biasPoint.x != 0 || biasPoint.y != 0) {
            result.translate(-biasPoint.x, -biasPoint.y);
        }
        if (s.x != 1 || s.y != 1) {
            result.scale(s.x, s.y);
        }
        if (r != 0) {
            result.rotate(r);
        }
        if (shear.x != 0) {
            result.shearX(shear.x);
        }
        if (shear.y != 0) {
            result.shearY(shear.y);
        }
        if (pos.x + biasPoint.x != 0 || pos.y + biasPoint.y != 0) {
            result.translate(pos.x + biasPoint.x, pos.y + biasPoint.y);
        }

        localToWorldMatrix = result;
    }

    /**
     * 更新逆矩阵
     */
    private void updateInverseMatrix() {
        Point2D s = new Point2D(scale);
        Point2D pos = new Point2D(position);

        double r = totalEffect(pos, rotation, s);

        AffineMatrix result = new AffineMatrix();

        // 逆变换次序：Tb^-1·T^-1 -> HY^-1 -> HX^-1 -> R^-1 -> S^-1 -> Tb
        if (biasPoint.x - pos.x != 0 || biasPoint.y - pos.y != 0) {
            result.translate(-biasPoint.x - pos.x, -biasPoint.y - pos.y);
        }
        if (shear.y != 0) {
            result.shearY(-shear.y);
        }
        if (shear.x != 0) {
            result.shearX(-shear.x);
        }
        if (r != 0) {
            result.rotate(-r);
        }
        // 如果原缩放积为0，不予逆缩放
        if ((s.x != 1 || s.y != 1) && (s.x != 0 && s.y != 0)) {
            result.scale(1 / s.x, 1 / s.y);
        }
        if (biasPoint.x != 0 || biasPoint.y != 0) {
            result.translate(biasPoint.x, biasPoint.y);
        }

        worldToLocalMatrix = result;
    }

    /**
     * 综合累积，顺便限制旋转。其中位移和缩放直接操作对象，旋转和是值传递故单独返回其值。
     * @param pos 位移，求和
     * @param r 旋转，求和
     * @param s 缩放，求积
     * @return 旋转和
     */
    private double totalEffect(Point2D pos, double r, Point2D s) {
        double rot = r;

        Transform t = parent, q = this;
        while (t != null) {
            if (q.i == I.ST || q.i == I.RST) s.scale(t.scale);
            if (q.i == I.RT || q.i == I.RST) rot += t.rotation;
            pos.add(t.position);
            q = t;
            t = t.parent;
        }

        // rotation的效果是周期性的，为了防止rotation绝对值过大，统一限制在[-360, 360]以内
        // 出口限制
        if (Math.abs(rot) > 360) {
            rot %= 360;
        }

        return rot;
    }

    /**
     * <h1>获取相对到世界仿射变换矩阵</h1>
     */
    public AffineMatrix getLocalToWorldMatrix() {
        if (isDirty) {
            updateMatrix();
            isDirty = false;
        }
        return localToWorldMatrix;
    }

    /**
     * <h1>获取世界到相对仿射变换矩阵</h1>
     */
    public AffineMatrix getWorldToLocalMatrix() {
        if (isInverseDirty) {
            updateInverseMatrix();
            isInverseDirty = false;
        }
        return worldToLocalMatrix;
    }

    /**
     * <h1>平移</h1>
     */
    public void translate(double dx, double dy) {
        position.x += dx;
        position.y += dy;
        markDirty();
        markInverseDirty();
    }

    /**
     * <h1>旋转</h1>
     */
    public void rotate(double deltaRadian) {
        rotation += deltaRadian;
        markDirty();
        markInverseDirty();
    }

    /**
     * <h1>缩放</h1>
     */
    public void scale(double dsx, double dsy) {
        scale.x += dsx;
        scale.y += dsy;
        markDirty();
        markInverseDirty();
    }

}
