# iostream-table
iostream-table is a simple c++ header only library that formats an output into a table.

# Usage
```cpp
#include <iostream>

#include "iostream-table.hpp"

int main()
{
  // Create a table
	ios_table::ios_table<std::string, float, float, int> 
		table({ "Function", "Avarge Time (ns)", "Total Time (ns)", "Iterations" });

  // Add rows to the table
	table.add_row("Get()", 1.1f, 1100.f, 1000);
	table.add_row("Write()", 1.2f, 13000.f, 10000);
	table.add_row("Push_Back()", 3.4f, 3400.f, 1000);

  // Output the table
  // Note: ios_table::to_string(table) is also a thing
	std::cout << table << '\n';

	std::cin.get();
	return 0;
}
```
# Example Output
```
|  Function   | Avarge Time (ns) | Total Time (ns) | Iterations |
|-------------|------------------|-----------------|------------|
|    Get()    |       1.10       |     1100.0      |    1000    |
|   Write()   |       1.20       |     13000.0     |   10000    |
| Push_Back() |       3.40       |     3400.0      |    1000    |
```
