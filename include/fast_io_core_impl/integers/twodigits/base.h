#pragma once

namespace fast_io
{

namespace details
{

template<char8_t base,bool uppercase,bool point=false,char32_t dec=u8'.',std::random_access_iterator Iter,typename U>
requires (!std::signed_integral<U>)
inline constexpr auto output_base_number_impl(Iter iter,U a)
{
//number: 0:48 9:57
//upper: 65 :A 70: F
//lower: 97 :a 102 :f
	constexpr auto &table(details::shared_static_base_table<std::iter_value_t<Iter>,base,uppercase>::table);
	constexpr std::uint32_t pw(static_cast<std::uint32_t>(table.size()));
	constexpr std::size_t chars(table.front().size());
	for(;pw<=a;)
	{
		auto const rem(a%pw);
		a/=pw;
		my_copy_n(table[rem].data(),chars,iter-=chars);
	}
	if constexpr(chars==2)
	{
		if(base<=a)
		{
			auto const& tm(table[a]);
			
			if constexpr(point)
			{
				*--iter=tm[1];
				*--iter=dec;
				*--iter=tm.front();
			}
			else
			{
				my_copy_n(tm.data(),chars,iter-=chars);
			}
		}
		else
		{
			if constexpr(point)
				*--iter=dec;
			if constexpr(10 < base)
			{
				if(a<10)
					*--iter = a+0x30;
				else
				{
					if constexpr (uppercase)
						*--iter = a+55;	
					else
						*--iter = a+87;
				}
			}
			else
				*--iter=a+0x30;
		}
	}
	else
	{
		if(base<=a)
		{
			auto const& tm(table[a]);
			auto i(tm.data());
			for(;*i==0x30;++i);
			auto const ed(tm.data()+chars);
			if constexpr(point)
			{
				my_copy(i+1,ed,iter-=ed-(i+1));
				*--iter=dec;
				*--iter=*i;
			}
			else
				my_copy(i,ed,iter-=ed-i);
		}
		else
		{
			if constexpr(point)
				*--iter=dec;
			if constexpr(10 < base)
			{
				if(a<10)
					*--iter = a+0x30;
				else
				{
					if constexpr (uppercase)
						*--iter = a+55;	
					else
						*--iter = a+87;
				}
			}
			else
				*--iter=a+0x30;
		}
	}
	return iter;
}


template<bool sign,std::uint8_t base>
requires (0x2<base&&base<=0x36)
struct is_numerical
{
template<std::integral T>
inline constexpr bool operator()(T ch)
{
	std::make_unsigned_t<T> e(ch);
	if constexpr(sign)
	{
		if constexpr(base<=0xA)
			return (e==0x2d)||static_cast<std::uint8_t>(e-0x30)<base;
		else
		{
			constexpr std::uint8_t basem10(base-0xa);
			return (e==0x2d)||static_cast<std::uint8_t>((e-0x30)<0xA)||
				(static_cast<std::uint8_t>(e-0x41)<basem10)||
				(static_cast<std::uint8_t>(e-0x61)<basem10);
		}
	}
	else
	{
		if constexpr(base<=0xA)
			return static_cast<std::uint8_t>(e-0x30)<base;
		else
		{
			constexpr std::uint8_t basem10(base-0xa);
			return (static_cast<std::uint8_t>(e-0x30)<0xA)||
				(static_cast<std::uint8_t>(e-0x41)<basem10)||
				(static_cast<std::uint8_t>(e-0x61)<basem10);
		}
	}
}
};
template<std::integral T>
inline constexpr bool is_space(T const u)
{
	if constexpr(std::unsigned_integral<std::remove_cvref_t<T>>)
	{
		return u==0x20||(u-0x9)<0x4;
	}
	else
	{
		std::make_unsigned_t<T> const e(u);
		return e==0x20||(e-0x9)<0x4;
	}
}


namespace twodigits
{
/*
template<std::unsigned_integral value>
constexpr inline int count_digits(uint64_t n)
{
	int t = (64 - FMT_BUILTIN_CLZLL(n | 1)) * 1233 >> 12;
	return t - (n < data::zero_or_powers_of_10_64[t]) + 1;
}
*/
template<std::contiguous_iterator Iter,my_unsigned_integral U>
constexpr inline std::uint32_t output_unsigned(Iter str,U value)
{
	constexpr auto tb_ptr(details::shared_static_base_table<std::iter_value_t<Iter>,10,false>::table.data());
	constexpr std::uint32_t val_size(2*sizeof(std::iter_value_t<Iter>));
	std::uint32_t const chars(chars_len<10>(value));
	auto i{std::to_address(str)+chars};
	for(;100<=value;)
	{
		memcpy(i-=2,tb_ptr+static_cast<std::uint32_t>(value%100),val_size);
		value/=100;
	}
	if(value<10)
	{
		i[-1]=static_cast<std::make_unsigned_t<std::iter_value_t<Iter>>>(value)+u8'0';
		return chars;
	}
	memcpy(i-2,tb_ptr+static_cast<std::uint32_t>(value),val_size);
	return chars;
}

}

}
}
