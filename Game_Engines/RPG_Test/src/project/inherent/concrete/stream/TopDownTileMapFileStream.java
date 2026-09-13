package project.inherent.concrete.stream;

import engine.base.LogUtils;
import project.inherent.old.GameRunner;
import project.inherent.concrete.dataItem.TopDownTileBlockItem;
import project.inherent.structure.stream.Separable;
import project.inherent.structure.stream.StreamOperations;
import project.inherent.tools.Parser;

import java.io.*;
import java.util.*;

/**
 * 俯视角平铺地图文件流
 */
public class TopDownTileMapFileStream implements StreamOperations<List<List<List<TopDownTileBlockItem>>>,
        List<List<TopDownTileBlockItem>>>, Separable<Integer> {
    private File dataFile;
    private final String baseFolder = "topDownMaps/";
    private final String extension = ".tileMap";
    private boolean isFileAccessible;

    public TopDownTileMapFileStream(String fileName) {
        dataFile = new File(baseFolder + fileName + extension);
        try {
            if (dataFile.createNewFile()) {
                LogUtils.fine("File created: " + dataFile.getName());
            } else {
                LogUtils.fine("File already exists.");
            }
            isFileAccessible = true;
        } catch (IOException e) {
            LogUtils.severe("An error occurred." + e);
            isFileAccessible = false;
        }
    }

    // TODO 俯视角平铺地图文件流实现方法

    /**
     * 读取地图数据。
     * <p>第一维：层分隔符 - |（从前至后层数增高）</p>
     * <p>第二维：行终止符 - \n</p>
     * <p>第三维：列分隔符 - ,</p>
     * @return 三维结构化地图数据。
     */
    @Override
    public List<List<List<TopDownTileBlockItem>>> read() {
        if (isFileAccessible) {
            List<List<List<TopDownTileBlockItem>>> blockItemMatrix3D = new ArrayList<>(4); // 默认四层
            Set<Integer> queryItemIdSet = new HashSet<>(); // 待查找的地图块id集合
            Map<Integer, TopDownTileBlockItem> fastId2ItemMap = new HashMap<>(); // 索引到数据项的快速映射
            // 使用BufferedReader逐行读取文件
            try (BufferedReader br = new BufferedReader(new FileReader(dataFile))) {
                String line;
                int curLayer = 0;
                // 逐行读取文件内容
                while ((line = br.readLine()) != null) {
                    if (line.contains("|")) {
                        curLayer++;
                        if (curLayer >= blockItemMatrix3D.size()) {
                            // TODO 扩容？
                        }
                        continue;
                    }
                    List<Integer> idInline = split(line, ","); // 读取一行中id的列表
                    queryItemIdSet.addAll(idInline);
                    // TODO 查找，存至fastId2ItemMap，清空queryItemIdSet

                    // TODO 如果fastId2ItemMap已有元素，直接将元素添加至blockItemMatrix3D
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

        } else {
            LogUtils.severe("Failed to read map file!");
        }

        return null;
    }

    @Override
    public void write(List<List<TopDownTileBlockItem>> in) {

    }

    @Override
    public boolean isFileAccessible() {
        return isFileAccessible;
    }

    /**
     * 解析输入字符串中的id并以列表方式返回。无法解析的，统一置1（虚空NilSpace）
     * @param in 输入
     * @param regex 分离依据正则字符串
     * @return 块id组
     */
    @Override
    public List<Integer> split(String in, String regex) {
        List<Integer> res = new ArrayList<>();
        String[] slices = in.split(regex);
        for (String s : slices) {
            res.add(Parser.parseIntOrDefault(s, 1));
        }
        return res;
    }

    public static void main(String[] args) {
        GameRunner.initLogger();
        // TODO 测试
    }
}
