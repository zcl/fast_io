#pragma once

namespace fast_io
{

template<input_stream T>
class posix_dev_urandom
{
public:
	using native_handle_type = T;
	using char_type = typename native_handle_type::char_type;
	native_handle_type device;
	template<typename... Args>
	requires ((sizeof...(Args)!=0)&&std::constructible_from<native_handle_type,Args...>)
	constexpr posix_dev_urandom(Args&& ...args):device(std::forward<Args>(args)...){}
	constexpr posix_dev_urandom():device("/dev/urandom",fast_io::open_interface<fast_io::open_mode::in|fast_io::open_mode::binary>){}
	constexpr auto& native_handle() noexcept
	{
		return device;
	}
	constexpr auto& native_handle() const noexcept
	{
		return device;
	}
};

template<input_stream T,std::contiguous_iterator Iter>
inline constexpr Iter read(posix_dev_urandom<T>& t,Iter begin,Iter end)
{
	return read(t.device,begin,end);
}

template<scatter_input_stream T,typename... Args>
inline constexpr auto scatter_read(posix_dev_urandom<T>& t,Args&& ...args)
{
	return scatter_read(t.device,std::forward<Args>(args)...);
}

template<typename T>
inline constexpr void require_secure_clear(posix_dev_urandom<T>){}

}