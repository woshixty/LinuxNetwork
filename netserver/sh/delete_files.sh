#!/bin/bash

# 检查参数数量
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <directory> <filename_pattern>"
    exit 1
fi

# 获取参数
TARGET_DIR=$1
FILENAME_PATTERN=$2

# 确认操作
read -p "Are you sure you want to delete files matching '$FILENAME_PATTERN' in '$TARGET_DIR'? [y/N]: " CONFIRM
if [[ "$CONFIRM" != "y" && "$CONFIRM" != "Y" ]]; then
    echo "Operation cancelled."
    exit 0
fi

# 执行查找并删除操作
find "$TARGET_DIR" -type f -name "$FILENAME_PATTERN" -exec rm -f {} +

# 提示完成
echo "Files matching '$FILENAME_PATTERN' in '$TARGET_DIR' have been deleted."
