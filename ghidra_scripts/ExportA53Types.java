// Exports the Ghidra data-type manager so exact DWARF layouts can be promoted to headers.
//@category A53

import java.io.BufferedWriter;
import java.io.File;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Iterator;

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.Composite;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.DataTypeComponent;
import ghidra.program.model.data.DataTypeManager;

public class ExportA53Types extends GhidraScript {

    private static String json(String value) {
        if (value == null) {
            return "";
        }
        return value.replace("\\", "\\\\").replace("\"", "\\\"")
                    .replace("\n", "\\n").replace("\r", "\\r");
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args.length != 1) {
            throw new IllegalArgumentException("Usage: ExportA53Types.java <output-directory>");
        }
        Path root = new File(args[0]).toPath();
        Files.createDirectories(root);
        DataTypeManager manager = currentProgram.getDataTypeManager();
        Iterator<DataType> types = manager.getAllDataTypes();
        int count = 0;
        int composites = 0;

        try (BufferedWriter writer = Files.newBufferedWriter(root.resolve("types.jsonl"),
                StandardCharsets.UTF_8)) {
            while (types.hasNext() && !monitor.isCancelled()) {
                DataType type = types.next();
                writer.write("{\"name\":\"" + json(type.getName()) + "\",\"path\":\""
                    + json(type.getDataTypePath().getPath()) + "\",\"display\":\""
                    + json(type.getDisplayName()) + "\",\"kind\":\""
                    + json(type.getClass().getSimpleName()) + "\",\"length\":" + type.getLength());
                if (type instanceof Composite) {
                    composites++;
                    Composite composite = (Composite) type;
                    writer.write(",\"components\":[");
                    DataTypeComponent[] components = composite.getDefinedComponents();
                    for (int i = 0; i < components.length; ++i) {
                        DataTypeComponent component = components[i];
                        if (i != 0) {
                            writer.write(',');
                        }
                        writer.write("{\"offset\":" + component.getOffset() + ",\"length\":"
                            + component.getLength() + ",\"field\":\""
                            + json(component.getFieldName()) + "\",\"type\":\""
                            + json(component.getDataType().getDisplayName()) + "\"}");
                    }
                    writer.write(']');
                }
                writer.write("}\n");
                count++;
            }
        }
        println("A53 type export complete: " + count + " types, " + composites + " composites");
    }
}
