package project.inherent.concrete.dataItem;

/**
 * 俯视角平铺地图项
 */
public class TopDownTileMapItem extends DataItem {
    /**
     * 名称。与地图文件（后缀名.map）文件名相同
     */
    private String name;
    /**
     * 创建时间
     */
    private String createDateTime;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getCreateDateTime() {
        return createDateTime;
    }

    public void setCreateDateTime(String createDateTime) {
        this.createDateTime = createDateTime;
    }
}
