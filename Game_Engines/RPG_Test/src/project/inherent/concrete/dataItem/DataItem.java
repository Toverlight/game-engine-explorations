package project.inherent.concrete.dataItem;

/**
 * 数据项。包含了主键(id)。
 */
public abstract class DataItem {
    /**
     * 任何数据项都有一个主键。主键均为自动递增。
     */
    protected int id;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }
}
