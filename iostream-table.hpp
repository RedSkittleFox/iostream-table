//	MIT License
//	
//	Copyright(c) 2019 Marcin Poloczek (RedSkittleFox)
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this softwareand associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright noticeand this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

// Author:	Marcin Poloczek (RedSkittleFox)
// License:	MIT
// Contact:	RedSkittleFox@gmail.com	|	https://github.com/RedSkittleFox
// Purpose:	IOSTREAM TABLE library

#ifndef IOSTREAM_TABLE_H_
#define IOSTREAM_TABLE_H_

#include <string>
#include <vector>
#include <tuple>
#include <ostream>
#include <array>
#include <type_traits>

namespace ios_table
{
	template<class... Args>
	class ios_table
	{
	private:
		using headers_type = typename std::array<std::string, sizeof...(Args)>;
	private:
		std::vector<std::tuple<Args...>> m_rows;
		headers_type m_headers;
	public:
		// Purpose:
		ios_table(headers_type _headers)
			:m_headers(std::move(_headers)) {}

		// Purpose:
		ios_table(const ios_table& other)
		{
			m_rows = other.m_rows;
			m_headers = other.m_headers;
		}

		// Purpose:
		ios_table(ios_table&& other)
		{
			m_rows = std::move(other.m_rows);
			m_headers = std::move(other.m_headers);
		}

		// Purpose: 
		ios_table& operator=(const ios_table& other)
		{
			m_rows = other.m_rows;
			m_headers = other.m_headers;
			return *this;
		}

		// Purpose: 
		ios_table& operator=(ios_table&& other)
		{
			m_rows = std::move(other.m_rows);
			m_headers = std::move(other.m_headers);
			return *this;
		}

	public:
		// Purpose: Add row to table
		void add_row(Args... _args)
		{
			m_rows.push_back(std::forward_as_tuple(_args...));
		}
		
		// Purpose: Ostream operator
		template<class... Args>
		friend std::ostream& operator<<(std::ostream& _os, const ios_table<Args...>& _table);

		// Purpose: Convert table to string
		template<class... Args>
		friend std::string to_string(ios_table<Args...>& _table);
	};

	namespace details
	{
		// Purpose: Convert any type to std::string
		template<class T>
		std::enable_if_t<std::is_integral_v<T>, std::string> format(T _t)
		{
			return std::to_string(_t);
		}

		// Purpose: Convert any type to std::string
		template<class T>
		std::enable_if_t<std::is_floating_point_v<T>, std::string> format(T _t)
		{
			// We want to remove trailing 0s
			std::string ret(std::to_string(_t));
			return ret.erase(ret.find_last_not_of('0') + 2, std::string::npos);
		}

		// Purpose: Convert any type to std::string
		std::string format(bool _b)
		{
			if (_b)
				return std::string("true");
			else
				return std::string("false");
		}

		// Purpose: Convert any type to std::string
		std::string format(char _c)
		{
			return std::string(1, _c);
		}

		// Purpose: Convert any type to std::string
		template<class T, class Traits, class Allocator>
		std::string format(std::basic_string<T, Traits, Allocator> _t)
		{
			return _t;
		}

		std::string to_formated_string(const std::string& _str, std::uint32_t _width)
		{
			std::string ret;
			ret.reserve(_width);
			ret.append((_width - _str.size()) / 2 + 1, ' ');
			ret.append(_str);
			ret.append((_width - _str.size()) / 2 + (_width - _str.size()) % 2 + 1, ' ');
			return ret;
		}

		// Purpose: Iterate over all elements of tuple
		template<std::size_t I = 0, class... Args>
		inline typename std::enable_if<I == sizeof...(Args), void>::type
			get_max(std::uint32_t* _column_sizes, std::tuple<Args...>& _tpl) {};

		// Purpose: Iterate over all elements of tuple
		template<std::size_t I = 0, class... Args>
		inline typename std::enable_if<I < sizeof...(Args), void>::type
			get_max(std::uint32_t* _column_sizes, std::tuple<Args...>& _tpl)
		{
			_column_sizes[I] = std::max(_column_sizes[I], format(std::get<I>(_tpl)).size());
			get_max<I + 1, Args...>(_column_sizes, _tpl);
		};

		// Purpose: Iterate over all elements of tuple
		template<std::size_t I = 0, class... Args>
		inline typename std::enable_if<I == sizeof...(Args), void>::type
			retrive_value(std::string& _str, std::uint32_t* _size, std::tuple<Args...>& _tpl) {};

		// Purpose: Iterate over all elements of tuple
		template<std::size_t I = 0, class... Args>
		inline typename std::enable_if < I < sizeof...(Args), void>::type
			retrive_value(std::string & _str, std::uint32_t* _size, std::tuple<Args...> & _tpl)
		{
			_str.append(to_formated_string(format(std::get<I>(_tpl)), _size[I]));
			_str.push_back('|');
			retrive_value<I + 1, Args...>(_str, _size, _tpl);
		};
	}

	// Purpose: Generate table string
	template<class... Args>
	std::string to_string(ios_table<Args...>& _table)
	{
		// We want to get sizes of each column first
		// They are adjusted to sizes of elements inside the column
		std::uint32_t column_sizes[sizeof...(Args)];

		for (std::uint32_t i = 0; i < sizeof...(Args); ++i)
		{
			column_sizes[i] = _table.m_headers[i].size();
		}

		for (std::uint32_t i = 0; i < _table.m_rows.size(); ++i)
		{
			details::get_max(column_sizes, _table.m_rows[i]);
		}

		std::uint32_t reserve{ 2 };

		for (std::uint32_t i = 0; i < sizeof...(Args); ++i)
		{
			reserve += column_sizes[i] + 1;
		}

		reserve *= (sizeof...(Args) + 2);

		std::string return_stream = "|";
		return_stream.reserve(reserve);

		// Start with putting header there
		for (std::uint32_t i = 0; i < sizeof...(Args); ++i)
		{
			return_stream.append(details::to_formated_string(_table.m_headers[i], column_sizes[i]));
			return_stream.push_back('|');
		}
		return_stream.push_back('\n');
		
		// Put divider
		return_stream.push_back('|');
		for (std::uint32_t i = 0; i < sizeof...(Args); ++i)
		{
			return_stream.append(std::string(column_sizes[i] + 2, '-'));
			return_stream.push_back('|');
		}
		return_stream.push_back('\n');

		// Push the rest of the values

		for (std::uint32_t i = 0; i < _table.m_rows.size(); ++i)
		{
			return_stream.push_back('|');
			details::retrive_value(return_stream, column_sizes, _table.m_rows[i]);
			return_stream.push_back('\n');
		}
		
		return_stream.push_back('\n');

		return return_stream;
	}

	// Purpose: Ostream operator
	template<class... Args>
	std::ostream& operator<<(std::ostream& _os, ios_table<Args...>& _table)
	{
		_os << to_string(_table);
		return _os;
	}
}

#endif
