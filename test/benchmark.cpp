#include <unit.h>
#include <iostream>
#include <tuple>
#include <chrono>
#include <type_traits>


template < typename Meter_t, typename Kilogram_t, typename Second_t, typename Velocity_t>
auto run(const Meter_t & m, const  Kilogram_t & kg, const Second_t & s, const Velocity_t &  v0) 
	//-> std::tuple< std::remove_reference_t<decltype(s)>, size_t, std::remove_reference_t<decltype(m)>> //,  std::remove_reference_t<decltype(kg*m*m/s/s)>  >
	-> std::tuple<size_t, Meter_t, Second_t, std::remove_reference_t<decltype(kg*m*m/s/s)> > //,  std::remove_reference_t<decltype(kg*m*m/s/s)>  >
{
	auto a = -9.86*m/s/s;
	auto dt = 1e-7*s;

	auto x = 0.0*m;
	auto t = 0.0*s;
	auto v = v0;

	size_t count =0;
	auto xmax = x;
	auto J = kg*m*m/s/s; //joule
	auto A = 0*J;
	while( double(x/m) >= 0 ) 
	{
		x = x + v*dt;
		v = v + a*dt;
		t = t+dt;
		if ( x > xmax) xmax = x;
		auto dA = a*kg*v*dt;
		if ( dA < 0*J) dA = -dA;
		if constexpr (std::is_same_v<decltype(A), double>) dA = std::abs(a*kg*v*dt);
		else dA = ibn::abs(a*kg*v*dt);
		A = A + dA;
		++count;
	}
	return {count, xmax, t, A};
};


int main(int argc, char ** argv) 
{
	double v0 = 100; 
	if (argc==2) v0= std::stod(argv[1]);
	auto begin_time = std::chrono::steady_clock::now();
#if defined (UNIT)
	struct Length {};
	struct Time {};
	struct Mass {}; 
	auto m = ibn::make_unit<Length>();
	auto s = ibn::make_unit<Time>();
	auto kg = ibn::make_unit<Mass>();
#endif
#if defined (DOUBLE)
	double m = 1.0;
	double s = 1.0;
	auto kg = 1.0;
#endif
	auto J = kg*m*m/s/s; //joule
	const auto [count,xmax, t, A] = run( m,  kg, s, v0*m/s);
	auto end_time =  std::chrono::steady_clock::now();
	std::cout << "Steps " << count << std::endl;
	std::cout << "Flight time: " << t/s << " seconds.\n";
	std::cout << "Max height : " << xmax/m << " meters.\n";
	std::cout << "Work : " << A/J << " joule.\n";
	//std::cout << "Flight time: t = "  << t/s << " seconds with "<< count <<  "  steps,  max height = " << xmax/m << " meters" << std::endl; //, work = " << A/J << " J"  << std::endl;
	//std::cout << "Flight time: t = "  << t/s << " seconds with "<< count <<  "  steps,  max height = " << xmax/m << " meters" << std::endl; //, work = " << A/J << " J"  << std::endl;
	std::cout << "Calculation time: " << std::chrono::duration_cast< std::chrono::duration<double> >(end_time -begin_time).count() << " seconds" << std::endl;
	return 0;
}
