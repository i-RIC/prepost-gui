#!/usr/bin/bash
echo "/* Add C includes here */

#if defined __cplusplus
/* Add C++ includes here */
" > $(ls *.pro | sed s/\.pro$/_pch.h/)
find -name "*.cpp" -o -name "*.h" | xargs egrep --no-filename include | egrep '<' | sort --unique >> $(ls *.pro | sed s/\.pro$/_pch.h/)
echo "
#endif // __cplusplus
" >> $(ls *.pro | sed s/\.pro$/_pch.h/)
code *.pro $(ls *.pro | sed s/\.pro$/_pch.h/)
echo "# Use Precompiled headers (PCH)
PRECOMPILED_HEADER = $(ls *.pro | sed s/\.pro$/_pch.h/)

######################" | clip




