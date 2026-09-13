package project.inherent.concrete.dataItem;

import engine.derive.predetermined.TileType;

/**
 * 俯视角平铺块项
 */
public class TopDownTileBlockItem extends DataItem {
    /**
     * 名称。与贴图的文件名一致
     */
    private String name;
    /**
     * 类型
     */
    private TileType tileType;
    /**
     * 其它特征。特征之间用分号隔开
     */
    private String otherFeatures;

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setTileType(TileType tileType) {
        this.tileType = tileType;
    }

    public TileType getTileType() {
        return tileType;
    }

    public void setOtherFeatures(String otherFeatures) {
        this.otherFeatures = otherFeatures;
    }

    public String getOtherFeatures() {
        return otherFeatures;
    }

}
