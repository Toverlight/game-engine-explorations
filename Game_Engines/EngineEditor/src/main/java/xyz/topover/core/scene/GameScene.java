package xyz.topover.core.scene;

import com.sun.prism.paint.Gradient;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.effect.BlendMode;
import javafx.scene.paint.Color;
import javafx.scene.paint.CycleMethod;
import javafx.scene.paint.LinearGradient;
import javafx.scene.paint.Stop;
import javafx.util.Pair;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import xyz.topover.core.calc.Point2D;
import xyz.topover.core.paint.*;
import xyz.topover.core.base.GameBase;
import xyz.topover.core.base.GameObject;
import xyz.topover.core.run.GameRunner;
import xyz.topover.core.view.CameraManager;

import java.util.*;
import java.util.stream.Collectors;

/**
 * <h1>游戏场景</h1>
 * <p>
 *     必须重载的方法中：<br>
 *     加载资源、初始化场景操作均在start内调用；<br>
 *     演出操作在update内调用；<br>
 *     卸载资源在destroy内调用。
 * </p>
 */
public abstract class GameScene extends GameBase {
    // TODO 背景层 List<GameObject>
    private final List<GameObject> gameObjects = new ArrayList<>(); // 更新场景内的所有对象
    private final List<SpriteRenderer> sprites = new ArrayList<>(); // 图形渲染
    private final List<LightSource> lights = new ArrayList<>(); // 控制阴影、暗面、光照
    private final List<Drawable> drawables = new ArrayList<>(); // 总绘制序列
    private double globalIntensity = 0.5; // 全局光照强度
    private Color ambient = Color.WHITE; // 环境光（默认白色）
    private boolean needsSorting = false; // 绘制序列是否需要排序的标记
    private boolean isLoaded = false; // 是否已经加载
    private static final Logger logger = LoggerFactory.getLogger(GameScene.class);

    @Override
    protected void logConstruction() {
        logger.debug("GameScene(id {}) created.", getId());
    }

    // --- 生命周期方法 --- //
    @Override
    public void start() {
        loadResources();
        initializeScene();
        isLoaded = true;
    }

    @Override
    public void update(double deltaTime) {
        if (!isActive() || !isLoaded) return;

        for (GameObject obj : gameObjects) {
            obj.update(deltaTime);
        }
        perform(deltaTime);
    }

    @Override
    public void destroy() {
        unloadResources();
        clearGameObjects();
        isLoaded = false;
    }

    // --- 场景特有方法 ---//

    /**
     * <h1>添加游戏对象</h1>
     * <p>
     *     触发游戏对象的初始化。<br>
     *     自动添加关联的绘制组件。
     * </p>
     * @param obj 游戏对象
     */
    public void addGameObject(GameObject obj) {
        gameObjects.add(obj);
        obj.start();
        LightSource light = obj.getComponent(LightSource.class);
        if (light != null) {
            lights.add(light);
            drawables.add(light);
            needsSorting = true;
        }
        SpriteRenderer sprite = obj.getComponent(SpriteRenderer.class);
        if (sprite != null) {
            sprites.add(sprite);
            drawables.add(sprite);
            needsSorting = true;
        }
    }

    /**
     * <h1>移除游戏对象</h1>
     * <p>
     *     触发游戏对象的销毁。<br>
     *     自动从场景列表移除与对象关联的绘制组件。
     * </p>
     * @param obj 游戏对象
     */
    public void removeGameObject(GameObject obj) {
        gameObjects.remove(obj);
        obj.destroy();
        LightSource light = obj.getComponent(LightSource.class);
        if (light != null) {
            lights.remove(light);
            drawables.remove(light);
            needsSorting = true;
        }
        SpriteRenderer sprite = obj.getComponent(SpriteRenderer.class);
        if (sprite != null) {
            sprites.remove(sprite);
            drawables.remove(sprite);
            needsSorting = true;
        }
    }

    /**
     * <h1>移除一群游戏对象</h1>
     * <p>
     *     其它效果同 removeGameObject 方法.
     * </p>
     * @param objsToRemove 游戏对象列表
     */
    public void removeManyGameObjects(List<GameObject> objsToRemove) {
        for (GameObject toRemove : objsToRemove) {
            for (Iterator<GameObject> itt = gameObjects.iterator(); itt.hasNext(); ) {
                GameObject toComp = itt.next();
                if (toComp == toRemove) {
                    SpriteRenderer s = toComp.getComponent(SpriteRenderer.class);
                    sprites.remove(s);
                    drawables.remove(s);
                    LightSource l = toComp.getComponent(LightSource.class);
                    lights.remove(l);
                    drawables.remove(l);
                    toRemove.destroy();
                    itt.remove();
                    if (!needsSorting && (s != null || l != null)) needsSorting = true;
                    break;
                }
            }
        }
    }

    /**
     * <h1>清空游戏对象</h1>
     * <p>
     *     触发所有游戏对象的销毁。<br>
     *     自动清空(光源,对象)对。
     * </p>
     */
    public void clearGameObjects() {
        for (GameObject o : gameObjects) {
            o.destroy();
        }
        gameObjects.clear();
        lights.clear();
        sprites.clear();
        drawables.clear();
    }

    // --- 光照相关 --- //

    /**
     * <h1>绘制阴影</h1>
     * <p>
     *     TODO 根据一些参数，决定绘制阴影的样子。
     *     投射在背景“墙”上阴影。有正影有斜影，由变换和渐变得到。<br>
     *     背景层上，物体之下。
     * </p>
     */
    public void renderShadow() {

    }

    /**
     * <h1>渲染图形（测试）</h1>
     * <p>
     *     物体本体的呈现。
     * </p>
     */
    public void renderShapes() {
        for (SpriteRenderer s : sprites) {
            if (s.isActive()) {
                s.draw(GameRunner.getGc());
            }
        }
    }

    /**
     * <h1>绘制暗面</h1>
     * <p>
     *     物体表面的阴影、暗区。由变换和渐变得到。<br>
     *     物体之上，光照之下。
     * </p>
     */
    public void renderDarkSurface(Drawable d) {
        if (lights.isEmpty()) return;
        if (d instanceof SpriteRenderer) {
            Shape s = ((SpriteRenderer) d).getShape();
            if (s instanceof Polygon) {
                // 获取要渲染的多边形
                Polygon polygon = (Polygon) s;
//                List<LinearGradient> linearGradients = new ArrayList<>();
                boolean[] isInfluencedByLights = new boolean[] {false};
                // 每个光源均对其施加作用
                for (LightSource light : lights) {
                    Point2D nearestWorld = polygon.nearest(light.getSrcPosition());
//                    Point2D nearest = CameraManager.getMainCamera().worldToCamera(nearestWorld);
                    double nearestDis = nearestWorld.dis(light.getSrcPosition());
                    if (nearestDis > light.getRadius()) continue;
//                    double nearestIntensity = FadeFuncLibrary.linear.calcIntensity(
//                            light.getRadius(), nearestDis,
//                            0, 0,
//                            light.getIntensity()
//                            );
//                    Point2D farthestWorld = polygon.farthest(light.getSrcPosition());
//                    Point2D farthest = CameraManager.getMainCamera().worldToCamera(farthestWorld);
//                    double farthestIntensity = FadeFuncLibrary.linear.calcIntensity(
//                            light.getRadius(), farthestWorld.dis(light.getSrcPosition()),
//                            0, 0,
//                            light.getIntensity()
//                    );
//                    Color colorNearest = Color.rgb(0, 0, 0, (1.0 - globalIntensity) * (1.0 - nearestIntensity));
//                    Color colorFarthest = Color.rgb(0,0,0,(1.0 - globalIntensity) * (1.0 - farthestIntensity));
//                    // 创建多光源线性渐变
//                    LinearGradient gradient = new LinearGradient(
//                            nearest.x, nearest.y,
//                            farthest.x, farthest.y,
//                            false,
//                            CycleMethod.NO_CYCLE,
//                            new Stop(0, colorNearest),
//                            new Stop(1, colorFarthest)
//                    );
//                    linearGradients.add(gradient);
                    if (!isInfluencedByLights[0]) isInfluencedByLights[0] = true;
                }

                GraphicsContext gc = GameRunner.getGc();
                // 在多边形的裁剪区域内渲染暗面
                polygon.funcInClip(() -> {
                    double canvasWidth = gc.getCanvas().getWidth();
                    double canvasHeight = gc.getCanvas().getHeight();
//                    gc.setGlobalBlendMode(BlendMode.MULTIPLY); // 使用乘法混合模式
//                    if (!isInfluencedByLights[0]) {
//                        // 绘制多光源渐变
//                        for (LinearGradient linearGradient : linearGradients) {
//                            gc.setFill(linearGradient);
//                            gc.fillRect(0, 0, canvasWidth, canvasHeight);
//                        }
                        // 绘制全局光照效果
                        gc.setFill(Color.rgb(0, 0, 0, 1.0 - globalIntensity));
                        gc.fillRect(0, 0, canvasWidth, canvasHeight);
//                    }
//                    gc.setGlobalBlendMode(BlendMode.SRC_OVER); // 恢复默认混合模式
                });
            }
        }
    }
    // 颜色插值函数
    private Color interpolateColor(Color ambient, Color light, double intensity) {
        double r = ambient.getRed() + intensity * (light.getRed() - ambient.getRed());
        double g = ambient.getGreen() + intensity * (light.getGreen() - ambient.getGreen());
        double b = ambient.getBlue() + intensity * (light.getBlue() - ambient.getBlue());
        double a = ambient.getOpacity() + intensity * (light.getOpacity() - ambient.getOpacity());
        return new Color(r, g, b, a);
    }

    /**
     * <h1>渲染光照效果（测试）</h1>
     * <p>
     *     照亮物体。由变换和渐变得到。<br>
     *     暗面之上，ui之下。
     * </p>
     */
    public void renderLighting() {
        for (LightSource light : lights) {
            if (light.isActive()) {
                light.draw(GameRunner.getGc());
            }
        }
    }

    /**
     * <h1>渲染全部</h1>
     * <p>
     *     <b>最推荐的渲染方案</b>。按照已按优先级排序好的绘制序列进行绘制。<br>
     *     已经包括了所有必要的渲染步骤和函数。
     * </p>
     */
    public void renderAll() {
        if (needsSorting) {
            drawables.sort(Comparator.comparingInt(Drawable::getLayer));
            needsSorting = false;
        }
        for (Drawable d : drawables) {
            d.draw(GameRunner.getGc());
            renderDarkSurface(d);
        }
    }

    /**
     * <h1>设置全局光照强度</h1>
     * @param globalIntensity 0.0~1.0。默认0.5.
     */
    public void setGlobalIntensity(double globalIntensity) {
        this.globalIntensity = Math.max(0, Math.min(1, globalIntensity));
    }

    public double getGlobalIntensity() {
        return globalIntensity;
    }

    /**
     * <h1>设置环境光颜色</h1>
     * @param ambient 环境光颜色。默认白色
     */
    public void setAmbient(Color ambient) {
        this.ambient = ambient;
    }

    public Color getAmbient() {
        return ambient;
    }

    //--- 抽象方法（子类实现具体逻辑）---//

    /**
     * 加载资源（如图片、音效）
     */
    protected abstract void loadResources();

    /**
     * 初始化场景布局
     */
    protected abstract void initializeScene();

    /**
     * 卸载资源
     */
    protected abstract void unloadResources();

    /**
     * 场景本身的更新（演出）
     */
    protected abstract void perform(double deltaTime);

    // --- 持久化相关 --- //

    /**
     * <h1>获取持久化对象列表</h1>
     */
    public List<Persistable> getPersistableS() {
        return gameObjects.stream()
                .filter(obj -> obj instanceof Persistable)
                .map(obj -> (Persistable) obj)
                .collect(Collectors.toList());
    }

    /**
     * <h1>恢复持久化对象</h1>
     * @param persistableS 持久化对象列表
     */
    public void restorePersistableS(List<Persistable> persistableS) {
        if (persistableS == null) {
            logger.debug("No persistable to be restored.");
            return;
        }
        for (Persistable obj : persistableS) {
            String key = obj.getPersistKey();
            Map<String, Object> data = GameStateManager.load(key);
            if (data != null) {
                obj.deserialize(data);
            } else {
                logger.error("Persistable(pkey {}) failed to restore, because data can't manage to load.", obj.getPersistKey());
            }

        }
    }

    /**
     * <h1>销毁场景中的非持久化对象</h1>
     */
    public void destroyNonPersistent() {
        List<GameObject> toRemove = gameObjects.stream()
                .filter(obj -> !(obj instanceof Persistable))
                .collect(Collectors.toList());
        toRemove.forEach(GameObject::destroy);
        removeManyGameObjects(toRemove);
    }
}