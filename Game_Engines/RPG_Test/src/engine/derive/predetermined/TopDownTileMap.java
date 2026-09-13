package engine.derive.predetermined;

import engine.base.*;
import engine.base.feature.TopDown;
import engine.base.old.Camera;
import engine.base.old.SceneMap;
import engine.base.old.SceneMapManager;

import java.util.ArrayList;
import java.util.List;

public abstract class TopDownTileMap<T extends TopDownTileBlock> extends SceneMap implements TopDown {
    private final List<List<T>> blocks; // 地图块信息矩阵
    private final Point2D startPos; // 起始位置
    private final Point2D blockSize; // 每个块的大小
    private boolean hasWarnedAboutBlock; // 是否触发过块为null的警告

    public TopDownTileMap(int width, int height, int blockWidth, int blockHeight) {
        startPos = new Point2D();
        blocks = new ArrayList<>(height);
        blockSize = new Point2D(blockWidth, blockHeight);
        for (int i = 0; i < height; i++) {
            blocks.add(new ArrayList<>(width));
            for (int j = 0; j < width; j++) {
                blocks.get(i).add(null);
            }
        }
        hasWarnedAboutBlock = false;
    }

    /**
     * TODO 以地图名从数据库加载地图（若加载失败，日志输出）
     */
    public void loadMap(String name) {

    }

    /**
     * TODO 保存地图（名称若重复，回退并日志输出）
     */
    public void saveMap() {

    }

    /**
     * 获取平铺块。
     * @param x 横坐标
     * @param y 纵坐标
     * @return 平铺块
     */
    public T getTile(int x, int y) {
        return blocks.get(y).get(x);
    }

    /**
     * 设置平铺块。
     * @param x 横坐标
     * @param y 纵坐标
     * @param tile 新的平铺块
     */
    public void setTile(int x, int y, T tile) {
        blocks.get(y).set(x, tile);
    }

    /**
     * 获取起始点信息的拷贝。
     */
    public Point2D getStartPos() {
        return new Point2D(startPos);
    }

    /**
     * 设置起始点信息。
     */
    public void setStartPos(int x, int y) {
        startPos.x = x;
        startPos.y = y;
    }

    /**
     * 渲染地图。
     */
    public final void Render() {
        if (!hasWarnedAboutBlock) {
            Camera camera = SceneMapManager.getCamera();
            Point2D showBlockSize = Point2D.scale(blockSize, camera.getZoomFactor());
            for (int y = 0; y < blocks.size(); y++) {
                for (int x = 0; x < blocks.size(); x++) {
                    TopDownTileBlock block = blocks.get(y).get(x);
                    if (block != null) {
                        Point2D leftTop = new Point2D(x * blockSize.x, y * blockSize.y);
                        Point2D cameraLeftTop = camera.worldToCamera(leftTop);
                        block.render(cameraLeftTop.x, cameraLeftTop.y, showBlockSize.x, showBlockSize.y);
                    } else {
                        warnAboutBlock(x, y);
                        hasWarnedAboutBlock = true;
                        return;
                    }
                }
            }
        }
    }

    /**
     * 检查指定位置能否通行。
     * @param x 横坐标
     * @param y 纵坐标
     */
    public boolean checkWalkable(int x, int y) {
        return blocks.get(y).get(x).isWalkable();
    }

    public void Update() {
        if (!hasWarnedAboutBlock) {
            for (int y = 0; y < blocks.size(); y++) {
                for (int x = 0; x < blocks.size(); x++) {
                    TopDownTileBlock block = blocks.get(y).get(x);
                    if (block != null) {
                        block.Update();
                    } else {
                        warnAboutBlock(x, y);
                        hasWarnedAboutBlock = true;
                        return;
                    }
                }
            }
        }
    }

    private void warnAboutBlock(int x, int y) {
        LogUtils.warning("TopDownTileMap(%d) with name '%s' detects there's block that" +
                "hasn't initialized and remains null at (%d,%d).\n\tIt's recommended to check" +
                " the loadMap method or other place called setTile method transferring " +
                "null however.\n\tBecause of this, Update and Render of the tile map has shut.",
                this.hashCode(), getName(), x, y);
    }


}
