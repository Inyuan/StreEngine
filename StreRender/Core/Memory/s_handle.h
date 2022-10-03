//#pragma once
//#include<string>
//
//#define HANDLE_MAX_NUMBER 256
//
//class res;
//
//static res* handle_table[HANDLE_MAX_NUMBER];
//static int handle_table_index = 0;
//
//
//
//class s_res_handle
//{
//public:
//
//
//private:
//	std::string uid;
//	int handle_index;
//public:
//	s_res_handle() = delete;
//
//	s_res_handle(res& in_t);
//
//	static int find_empty_handle(res* in_res)
//	{
//
//		handle_table_index %= HANDLE_MAX_NUMBER;
//
//		unsigned int count = 0;
//
//		while (handle_table[handle_table_index])
//		{
//			handle_table_index++;
//			handle_table_index %= HANDLE_MAX_NUMBER;
//			count++;
//		}
//
//		if (count >= HANDLE_MAX_NUMBER)
//		{
//			return -1;
//		}
//
//		handle_table[handle_table_index] = in_res;
//
//		return handle_table_index;
//	}
//
//	static void delete_handle(int in_handle_index)
//	{
//		handle_table[in_handle_index] = nullptr;
//	}
//
//	res* get_ptr();
//
//public:
//
//
//
//};
//
//
//
//class res
//{
//private:
//	std::string uid;
//	int handle_index;
//
//	friend class s_res_handle;
//
//	static int res_index;
//public:
//
//	res()
//	{
//		uid = "res_" + std::to_string(res_index);
//
//		handle_index = s_res_handle::find_empty_handle(this);
//	}
//
//	~res()
//	{
//		s_res_handle::delete_handle(handle_index);
//	}
//};
//
//int res::res_index = 0;
//
//
//s_res_handle::s_res_handle(res& in_t) : handle_index(in_t.handle_index), uid(in_t.uid)
//{
//
//}
//
//res* s_res_handle::get_ptr()
//{
//	if (handle_table[handle_index] && handle_table[handle_index]->uid == uid)
//	{
//		return handle_table[handle_index];
//	}
//	return nullptr;
//}