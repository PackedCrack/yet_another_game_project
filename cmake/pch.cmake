SET(STD_PCH <cstdint>
<cstdio>
<iostream>
<fstream>
<string>
<string_view>
<array>
<vector>
<unordered_map>
<map>
        <execution>
<list>
<stack>
<set>
<unordered_set>
<queue>
<functional>
<utility>
<optional>
<memory>
<chrono>
<bitset>
<concepts>
<random>
<algorithm>
<exception>
<type_traits>
<filesystem>
<concepts>)

# --- functions --- #
function(use_pch PROJ)
  target_precompile_headers(${PROJ}
    PRIVATE
    ${STD_PCH}
  )
endfunction()
