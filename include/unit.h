/*
 * =====================================================================================
 *
 *       Filename:  unit.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  19.09.2023 19:22:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ivan B. Nikolaev (ekherit), I.B.Nikolaev@inp.nsk.su
 *   Organization:  Budker Insitute of Nuclear Physics
 *
 * =====================================================================================
 */

#ifndef IBN_UNIT_H
#define IBN_UNIT_H

#include <ratio>
#include <cmath>
#include <string.h>
#include <string_view>

namespace ibn 
{
	//helper function (implementation dependant) to determine type name in compile time
	template<class T>
	constexpr std::string_view get_name()
	{
		char const* p = __PRETTY_FUNCTION__;
		while (*p++ != '=');
		for (; *p == ' '; ++p);
		char const* p2 = p;
		int count = 1;
		for (;;++p2) {
			if(*p2==';') break;
		}
		return {p, std::size_t(p2-p)};
	};

	template<typename BaseType, typename Power = std::ratio<1,1> >
	struct BaseDimension : public Power
	{
		using type = BaseType;
	};

	//complex dimension is a series of base dimensions
	template<typename ...Bs>
	struct Dimension
	{
		//static const int size = sizeof...(Bs); //number of base dimensions in complex dimension
	};

	// ************* OPERATION WITH DIMENSIONS *************************************** 

	//concatenation without manipulation 
	template<typename D1, typename D2>         struct Concatenate;
	template<typename ...Bs1, typename... Bs2> struct Concatenate< Dimension<Bs1...>, Dimension<Bs2...> > { using type = Dimension<Bs1...,Bs2...>; };
	template<typename D1, typename D2> using Concatenate_t  = typename Concatenate<D1,D2>::type;

	//multiply dimensions
	template<typename D, typename ... Ds> struct Multiply;
	template<typename ... Bs>          struct Multiply< Dimension<>, Dimension<Bs...> > { using type = Dimension<Bs...>; };
	template<typename B>               struct Multiply< Dimension<B>, Dimension<> >     { using type = Dimension<B>; };

	template <typename ... Ds> using Multiply_t = typename Multiply<Ds...>::type;

	template<typename ... Ds>          struct Multiply< Dimension<>, Ds... > { using type = Multiply_t<Ds...>; };
	
	template<typename B1, typename B2, typename ... Bs> 
	struct Multiply< Dimension<B1>, Dimension<B2, Bs...> > 
	{
		using type =  typename std::conditional 
							< std::is_same_v<typename B1::type, typename B2::type>,  //if B1 and B2 are same base dimension
								//then sum dimensions and shrink total list
								typename std::conditional
									< std::ratio_add<B1,B2>::num == 0,
										Dimension<Bs...>,
										Dimension<BaseDimension<typename B1::type, typename std::ratio_add<B1,B2>::type>, Bs...>
									>::type,
								//otherwice compare base dimension name
								typename std::conditional 
									< get_name<typename B1::type>() < get_name<typename B2::type>(),
										Concatenate_t< Dimension<B1> , Dimension<B2, Bs...> >,
										Concatenate_t< Dimension<B2> ,  Multiply_t< Dimension<B1>, Dimension<Bs...> > >
									>::type
							>::type;

	};

	template<typename B1, typename ... Bs1, typename ... Bs2> 
	struct Multiply< Dimension<B1, Bs1...>, Dimension<Bs2...> > 
	{
		using type =  Multiply_t< Dimension<Bs1...>,  Multiply_t<Dimension<B1>,  Dimension<Bs2...>  > >;
	};

	template<typename D1, typename D2, typename ... Ds> 
	struct Multiply < D1, D2, Ds...> 
	{
		using type =  Multiply_t< D1,  Multiply_t<D2,  Ds...  > >;
	};


	//inverse element
	template<typename B>
	struct Inverse;

	template<>
	struct Inverse<Dimension<>> 
	{
		using type = Dimension<>;
	};

	template<typename B> using Inverse_t = typename Inverse<B>::type;

	//inverse element of base dimension
	template <typename BaseType, typename R>
	struct Inverse<Dimension<BaseDimension<BaseType,R>>>
	{
		private:
			using sub = typename std::ratio_subtract< std::ratio<0,1>, R >::type; 
		public:
			using type = Dimension<BaseDimension<BaseType,sub>>;
	};

	//inverse element of complex dimension
	template <typename B1, typename ... Bs>
	struct Inverse< Dimension<B1,Bs...> >
	{
			using type = Concatenate_t< Inverse_t<Dimension<B1>>,  Inverse_t<Dimension<Bs...>>>;
	};

	//power operator
	template<typename B, int n, int m=1>
	struct Power;

	template<int n, int m> 
	struct Power< Dimension<>, n, m> 
	{
		using type = Dimension<>;
	};

	template<typename B, int n, int m=1> using Power_t = typename  Power<B,n,m>::type;

	template<typename B1, int n, int m, typename...Bs>
	struct Power< Dimension<B1, Bs...>, n, m> 
	{
		private:
			using B1n = Dimension< BaseDimension<typename B1::type, std::ratio_multiply<B1,std::ratio<n,m>>> >;
			using Bsn = Power_t<Dimension<Bs...>,n, m>;
		public:
			using type = Multiply_t<B1n, Bsn>;
	};


	//some usefull functions
	template <typename U> 
	struct is_dimensionless;

	template <> 
	struct is_dimensionless<Dimension<>>
	{
		static const bool value = true;
	};

	template < typename B, typename ...Bs > 
	struct is_dimensionless< Dimension<B, Bs...> >
	{
		static const bool value = is_dimensionless<Dimension<Bs...> >::value && (B::num == 0);
	};



	template< typename D, typename Field = double >
	struct unit
	{
		private:
			constexpr unit(Field f) noexcept: data{f} {} 
		public:
			using dimension = D;
			using field=Field;
			using type = unit<dimension, Field>;

			constexpr unit(void)noexcept  : data{1.0} {};

			constexpr type & operator=(const unit & t) noexcept 
			{ 
				data = t.data;
				return *this;
			}

			constexpr friend type operator*(const field & f, const type  & t) noexcept { return {f*t.data}; }
			constexpr friend type operator*(const  type & t, const field & f) noexcept { return {t.data*f}; }

			constexpr friend type operator+(const type & t1, const type & t2) noexcept { return {t1.data + t2.data}; }
			constexpr friend type operator-(const type & t1, const type & t2) noexcept { return {t1.data - t2.data}; }

			constexpr friend type operator/(const type & t, const field & f) { return {t.data/f}; }
			constexpr friend auto operator/(const field & f, const type & t) -> unit<typename Inverse<dimension>::type, field> { return f/t.data; }

			constexpr type & operator+=(const type & t) { data+=t.data; return *this; }


			//constexpr friend bool operator<(const type & t1, const type &t2) { return t1.data < t2.data; }
			//constexpr friend bool operator>(const type & t1, const type &t2) { return t1.data > t2.data; }

			//constexpr auto operator<=>(const type & t) const = default;
			constexpr friend auto operator<=>(const type & t1, const type &t2) noexcept { return t1.data <=> t2.data; }


			constexpr friend type operator-(const type & t1) noexcept { return {-t1.data}; }


			constexpr operator typename std::conditional< is_dimensionless<dimension>::value, field, type>::type () noexcept { return data; };

			template<int Num,int Den=1>
			constexpr auto pow(void) noexcept -> unit<Power_t<dimension, Num, Den>,field> 
			{ 
				return static_cast<field>(::pow(data,static_cast<double>(Num)/Den));
			}



		private:


		public:
			field data;
	};


	template < typename T1, typename T2>
	inline constexpr auto operator*(const T1 & t1 , const T2 & t2 ) -> unit<Multiply_t<typename T1::dimension, typename T2::dimension>, typename T1::field>
	{
		return (t1.data*t2.data)*unit<Multiply_t<typename T1::dimension, typename T2::dimension>, typename T1::field>();
	}

	template < typename D1, typename D2, typename Field>
	inline constexpr auto operator/(const unit<D1, Field> & t1 , const unit<D2, Field> & t2 ) -> unit<typename Multiply<D1, typename Inverse<D2>::type >::type, Field>
	{
		return (t1.data/t2.data)*unit<Multiply_t<D1, Inverse_t<D2> >, Field>();
	}

	template<typename D>
	inline constexpr D abs(const D & d) 
	{
		typename D::type t;
		t.data = std::abs(d.data);
		return t;
	}
	


	template<typename D, typename Field> 
	inline constexpr auto sqrt(const unit<D,Field> & u) -> unit<  Power_t< typename unit<D,Field>::dimension, 1, 2>, Field >
	{
		return sqrt(u.data)*unit<  Power_t< typename unit<D,Field>::dimension, 1, 2>, Field >();
	}


	template<typename  BaseType, long Num=1, long Den=1>  
	using make_dimension_t = Dimension< BaseDimension<BaseType, std::ratio<Num,Den> > >;

	template<typename Id, typename Field=double>
	using make_unit = unit< Dimension<BaseDimension<Id> >, Field>; 

}
#endif
