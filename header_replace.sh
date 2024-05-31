#!/bin/bash

# 检查输入参数
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

DIRECTORY=$1

# 新的注释块内容
NEW_HEADER="/*
* SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
*
* SPDX-License-Identifier: MIT
*/"

# 临时文件用于存储替换后的内容
TEMP_FILE=$(mktemp)

# 遍历文件夹中的所有 *.c, *.cpp, *.h 文件
find "$DIRECTORY" -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) | while read -r FILE
do
    # 检查文件是否包含旧的注释块
    if grep -q "^/\*\*" "$FILE"; then
        # 使用 awk 处理文件头部的注释块替换
        awk -v new_header="$NEW_HEADER" '
        BEGIN { print new_header; skip = 1 }
        skip && /\/\*\*$/ { skip = 1; next }
        skip && /\*\// { skip = 0; next }
        !skip { print }
        ' "$FILE" > "$TEMP_FILE"

        # 将替换后的内容写回文件
        mv "$TEMP_FILE" "$FILE"
        echo "Processed $FILE"
    else
        echo "Skipped $FILE"
    fi
done

# 清理临时文件
rm -f "$TEMP_FILE"

echo "Replacement complete"
