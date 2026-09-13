package xyz.topover.core.entity;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.base.GameComponent;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.paint.CollisionBox;
import xyz.topover.core.paint.Rectangle;

/**
 * <h1>物理组件</h1>
 */
public class Physics extends GameComponent {
    private double vx = 0;
    private double vy = 0;
    private double fx = 0;
    private double fy = 0;
    private double m = 1;
    private double omega = 0; // 角速度
    private CollisionBox collisionBox;
    private CollisionInfo collisionInfo; // null时表示无碰撞
    private double e = 1; // 弹性系数
    private double mu = 0; // 摩擦因子
    public static final double MASS_MAX = 1e100;
    public static final double MASS_MIN = 1e-10;
    private Physics lastPhysics;

    private static final Logger logger = LoggerFactory.getLogger(Physics.class);

    /**
     * <h1>碰撞信息</h1>
     * @param collisionAxis 碰撞法线
     * @param minOverlap 最小深度
     * @param physics 另一个物理组件
     * @param collisionPoint 碰撞点
     */
    public record CollisionInfo(Point2D collisionAxis, double minOverlap, Physics physics, Point2D collisionPoint) {}

    /**
     * <p>
     *     注意：传入的碰撞形状需提前指定transform.且不能有父transform.否则会出现未定义行为.
     * </p>
     * @param collisionBox 碰撞形状
     */
    public Physics(CollisionBox collisionBox) {
        this.collisionBox = collisionBox;
    }

    @Override
    public void start() {
        PhysicsHandler.registerPhysics(this);
        logger.debug("Physics(id {}) started.", getId());
    }

    public Physics setCollisionBox(CollisionBox collisionBox) {
        this.collisionBox = collisionBox;
        return this;
    }

    public CollisionBox getCollisionBox() {
        return collisionBox;
    }

    public void setCollisionInfo(CollisionInfo collisionInfo) {
        this.collisionInfo = collisionInfo;
    }
    public CollisionInfo getCollisionInfo() {
        return collisionInfo;
    }

    /**
     * <h1>设置质量(kg)</h1>
     * <p>
     *     自动限制在1e-10 ~ 1e100之间。
     * </p>
     */
    public Physics setM(double m) {
        this.m = Math.max(MASS_MIN, Math.min(MASS_MAX, m));
        return this;
    }

    public Physics applyForce(double fx, double fy) {
        this.fx += fx;
        this.fy += fy;
        return this;
    }
    public Physics applyForce(Point2D force) {
        this.fx += force.x;
        this.fy += force.y;
        return this;
    }

    public Physics setVx(double vx) {
        this.vx = vx;
        return this;
    }
    public Physics setVy(double vy) {
        this.vy = vy;
        return this;
    }
    public Physics setV(Point2D v) {
        this.vx = v.x;
        this.vy = v.y;
        return this;
    }
    public Physics setOmega(double omega) {
        this.omega = omega;
        return this;
    }

    /**
     * <h1>设置恢复系数</h1>
     * <p>
     *     碰撞采取sqrt(e1*e2)的公式得到等效恢复系数。
     * </p>
     * @param e 0~1之间的取值（自动限制）
     */
    public Physics setE(double e) {
        this.e = Math.max(0, Math.min(1, e));
        return this;
    }
    /**
     * <h1>设置摩擦系数</h1>
     * <p>
     *     摩擦采取sqrt(mu1*mu2)的公式得到等效摩擦系数。
     * </p>
     * @param mu 0~1之间的取值（自动限制）
     */
    public Physics setMu(double mu) {
        this.mu = Math.max(0, Math.min(1, mu));
        return this;
    }

    public double getOmega() {
        return omega;
    }
    public double getVx() {
        return vx;
    }
    public double getVy() {
        return vy;
    }

    public static void checkCollision(Physics pa, Physics pb) {
        if (!pa.isActive() || !pb.isActive()) return;
        if (pa.collisionInfo != null && pb.collisionInfo != null) return;
        CollisionBox boxA = pa.getCollisionBox();
        CollisionBox boxB = pb.getCollisionBox();
        double minOverlap = Double.MAX_VALUE;
        Point2D collisionAxis = null;
        Point2D[] worldVerticesA = boxA.getWorldVertices();
        Point2D[] worldVerticesB = boxB.getWorldVertices();
        Point2D[] axesA = boxA.getAxes(worldVerticesA);
        Point2D[] axesB = boxB.getAxes(worldVerticesB);
        Point2D[] axes = new Point2D[axesA.length + axesB.length];
        System.arraycopy(axesA, 0, axes, 0, axesA.length);
        System.arraycopy(axesB, 0, axes, axesA.length, axesB.length);
        Point2D[] pointsMinMaxA = new Point2D[2];
        Point2D[] pointsMinMaxB = new Point2D[2];
        boolean isColliding = false;
        for (Point2D axis : axes) {
            Point2D[] minMaxA = boxA.projectMinMax(worldVerticesA, axis);
            Point2D[] minMaxB = boxB.projectMinMax(worldVerticesB, axis);
            double overlap = Math.min(minMaxA[0].y, minMaxB[0].y) - Math.max(minMaxA[0].x, minMaxB[0].x);
            if (overlap < 0) {
                if (pa.collisionInfo == null) {
                    pa.lastPhysics = null;
                }
                if (pb.collisionInfo == null) {
                    pb.lastPhysics = null;
                }
                return;
            }
            if (overlap < minOverlap) {
                minOverlap = overlap;
                collisionAxis = axis;
                pointsMinMaxA[0] = minMaxA[1];
                pointsMinMaxB[0] = minMaxB[1];
                pointsMinMaxA[1] = minMaxA[2];
                pointsMinMaxB[1] = minMaxB[2];
                isColliding = true;
            }
        }
        if (isColliding) {
            Point2D collisionPoint = Point2D.scale(Point2D.plus(
                    (pointsMinMaxA[1].dot(collisionAxis) < pointsMinMaxB[1].dot(collisionAxis) ? pointsMinMaxA[1] : pointsMinMaxB[1]),
                    (pointsMinMaxA[0].dot(collisionAxis) > pointsMinMaxB[0].dot(collisionAxis) ? pointsMinMaxA[0] : pointsMinMaxB[0])
            ), 0.5);
            collisionAxis = (Point2D.minus(pb.collisionBox.getPosition(), pa.collisionBox.getPosition()).dot(collisionAxis) > 0
                    ? collisionAxis : Point2D.invert(collisionAxis));
            if (pa.collisionInfo == null) {
                if (pa.lastPhysics != pb) {
                    pa.setCollisionInfo(new CollisionInfo(collisionAxis, minOverlap, pb, collisionPoint));
                    pa.lastPhysics = pb;
                }
            }
            if (pb.collisionInfo == null) {
                if (pb.lastPhysics != pa) {
                    pb.setCollisionInfo(new CollisionInfo(collisionAxis, minOverlap, pa, collisionPoint));
                    pb.lastPhysics = pa;
                }
            }
        }
    }

    @Override
    public void update(double deltaTime) {
        if (isActive()) {
            if (collisionInfo != null) {
                Physics other = collisionInfo.physics;
                Point2D r1 = Point2D.minus(collisionInfo.collisionPoint, collisionBox.getPosition());
                Point2D r2 = Point2D.minus(other.collisionInfo.collisionPoint, other.collisionBox.getPosition());
                double I1 = m * r1.dot(r1);
                double I2 = other.m * r2.dot(r2);
                Point2D n = collisionInfo.collisionAxis;
                Point2D t = Point2D.rotate(n, Math.PI / 2.0);
                Point2D v1 = new Point2D(vx, vy);
                Point2D v2 = new Point2D(other.vx, other.vy);

                Point2D vp1 = Point2D.plus(v1, Point2D.scale(r1, omega));
                Point2D vp2 = Point2D.plus(v2, Point2D.scale(r2, other.omega));

                Point2D vrel = Point2D.minus(vp2, vp1);

                double vreln = vrel.dot(n);
                double vrelt = vrel.dot(t);

                double ae = Math.sqrt(e * other.e);
                double amu = Math.sqrt(mu * other.mu);

                double Jn = -(1.0 + ae) * vreln / (1.0 / m + 1.0 / other.m + r1.cross(n) * r1.cross(n) / I1 + r2.cross(n) * r2.cross(n) / I2);
                double meff = 1 / (1.0 / m + 1.0 / other.m + r1.cross(t) * r1.cross(t) / I1 + r2.cross(t) * r2.cross(t) / I2);
                double Jt = (vrelt > 0 ? -1 : 1) * Math.min(Math.abs(amu * Jn), Math.abs(meff * vrelt));

                Point2D v1_ = Point2D.plusAll(v1, Point2D.scale(n, Jn / m), Point2D.scale(t, Jt));
                Point2D v2_ = Point2D.minus(v2, Point2D.plus(Point2D.scale(n, Jn / other.m), Point2D.scale(t, Jt)));

                double omega1_ = omega + r1.cross(Point2D.plus(Point2D.scale(n, Jn), Point2D.scale(t, Jt))) / I1;
                double omega2_ = other.omega - r2.cross(Point2D.plus(Point2D.scale(n, Jn), Point2D.scale(t, Jt))) / I2;

                vx = v1_.x;
                vy = v1_.y;
                other.vx = v2_.x;
                other.vy = v2_.y;
                omega = omega1_;
                other.omega = omega2_;

                Point2D fix = Point2D.scale(collisionInfo.collisionAxis, collisionInfo.minOverlap); // self -> other
                double fixRatio = other.m / (m + other.m);
                gameObject.getTransform().translate(-fix.x * fixRatio, -fix.y * fixRatio);
                other.gameObject.getTransform().translate(fix.x * (1 - fixRatio), fix.y * (1 - fixRatio));

                collisionInfo = null;
                other.collisionInfo = null;
            }

            vx += fx * deltaTime / m;
            vy += fy * deltaTime / m;
            gameObject.getTransform().translate(vx * deltaTime, vy * deltaTime);
            fx = 0;
            fy = 0;
            collisionBox.rotate(omega * deltaTime);
        }
    }
}
