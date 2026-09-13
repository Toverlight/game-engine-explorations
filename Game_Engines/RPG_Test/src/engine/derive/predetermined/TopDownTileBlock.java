package engine.derive.predetermined;

import engine.base.*;
import engine.base.old.Camera;
import engine.base.old.Obj;
import engine.base.old.SceneMapManager;
import javafx.scene.image.Image;
import project.inherent.old.GameRunner;

public abstract class TopDownTileBlock extends Obj {
    private TileType tileType;
    private boolean walkable;
    private Image image;
    private boolean hasWarnedAboutImage;

    public TopDownTileBlock(TileType tileType, boolean walkable, Image image) {
        this.tileType = tileType;
        this.walkable = walkable;
        this.image = image;
        hasWarnedAboutImage = false;
    }

    public boolean isWalkable() {
        return walkable;
    }

    public void setImage(Image image) {
        this.image = image;
    }

    /**
     * 渲染平铺块。规定该函数由TopDownTileMap统一调用。
     * 传入的参数均为显示参数而非绝对参数。
     * @param x 渲染的横向起始位置（左界）
     * @param y 渲染的纵向起始位置（上界）
     * @param width 渲染的宽度
     * @param height 渲染的高度
     */
    public void render(double x, double y, double width, double height) {
        if (image != null) {
            GameRunner.getGc().drawImage(image, x, y, width, height);
        } else {
            if (!hasWarnedAboutImage) {
                Camera camera = SceneMapManager.getCamera();
                Point2D oriPos = camera.cameraToWorld(new Point2D(x, y));
                LogUtils.warning("TopDownTileBlock(%d) with name '%s' at " + oriPos +
                        " renders nothing because its image has been set null by <init> or setImage method.",
                        this.hashCode(), getName());
                hasWarnedAboutImage = true;
            }
        }
    }

}
