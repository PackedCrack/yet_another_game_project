SET(STD_PCH <cstdint>
<algorithm>
<array>
<bitset>
<chrono>
<concepts>
<cstdio>
<exception>
<execution>
<expected>
<filesystem>
<fstream>
<functional>
<iostream>
<list>
<map>
<memory>
<optional>
<queue>
<random>
<set>
<string>
<string_view>
<stack>
<span>
<type_traits>
<utility>
<unordered_map>
<unordered_set>
<variant>
<vector>
)

# --- functions --- #
function(use_pch PROJ)
  target_precompile_headers(${PROJ}
    PRIVATE
    ${STD_PCH}
  )
endfunction()
