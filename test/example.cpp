#include <unit.h>
#include <type_traits>
#include <iostream>
#include <cxxabi.h>
#include <string>
#include <numbers>
#include <iomanip>

template<typename T> std::string name(void) {
	int status;
	return std::string(abi::__cxa_demangle( typeid(T).name(), NULL, NULL, &status) );
};


int main( int argc, char ** argv) {
	//declare a tag or name for new dimension
	struct Length {};
	//declare dimension type ( verbose way) 
	using  LengthDimension = ibn::Dimension< ibn::BaseDimension<Length> > ;
	
	struct Time {};
	//declare dimension type (short way)
	using  TimeDimension = ibn::make_dimension_t<Time>; 


	struct Mass {}; 
	using  MassDimension = ibn::make_dimension_t<Mass>; 


	//declare derivated dimensions
	using  FrequencyDimension = ibn::Inverse_t<TimeDimension>;

	//declare combination of dimensions
	using  VelocityDimension = ibn::Multiply_t< LengthDimension, ibn::Power_t< TimeDimension, -1> >;

	//type above are just dimension and dont care the data value

	//define 1 meter or etalon over double field
	constexpr ibn::unit<LengthDimension, double> m;

	//onother way to define
	constexpr auto s = ibn::make_unit<Time>();
	constexpr auto kg = ibn::make_unit<Mass>();


	//declare Energy type from etalon units
	using EnergyDimension = ibn::Multiply_t<MassDimension,  ibn::Power_t< LengthDimension, 2>, ibn::Power_t<TimeDimension, -2>>;
	using Joule_t = ibn::unit< EnergyDimension> ;
	constexpr auto J = kg*m*m/s/s;
	std::cout << "J type :       " << name<decltype(J) >() << std::endl;
	std::cout << "Joule_t type : " << name< Joule_t >() << std::endl;
	static_assert( std::is_same_v<Joule_t, decltype(kg*m*m/s/s) >  );
	static_assert( std::is_same_v<Joule_t, std::remove_cv_t<decltype(J)> >  );

	std::cout << std::setprecision(15);


	//light speed
	constexpr auto c = 299792458.*m/s; //exact
	//define 1 Coulomb
	struct Charge {};
	constexpr auto C =  ibn::make_unit<Charge>();
	//electron charge
	constexpr auto e = 1.602'176'634e-19*C; //exact
	//declare Volts
	constexpr auto V = J/C;

	//calculate eV
	constexpr auto eV = e*V;
	std::cout << " 1eV = " << eV/J  << " J" << std::endl;

	constexpr auto MeV = 1e6*eV;

	//electron mass in energy units
	
	constexpr auto me = 0.510'998'950'0*MeV; 

	std::cout << "Electron mass = " << me/MeV << " MeV = " << me/c/c/kg << " kg " << std::endl;

	//plank constant
	constexpr auto h = 6.626'070'15e-34*J*s; //exact
	std::cout << "Plank constant: " << h/J/s << " J*s" << std::endl;
	constexpr auto hbar =h/(2*std::numbers::pi); 

	constexpr auto hc = hbar*c;

	constexpr auto fm = 1e-15*m;
	std::cout << " hc = " << hc/MeV/(fm) << " MeV*fm " << std::endl;


	constexpr auto N = J/m; //force newton
	constexpr auto A = C/s; //elecric current Amper

	//permeability of free space
	constexpr auto mu0 =  4*std::numbers::pi*1e-7*1.000'000'000'55*N/A/A;
	//permittivity of free space
	constexpr auto eps0 = 1./(mu0*c*c);


	std::cout << "permittivity of free space = " << eps0/ decltype(eps0)()  << "  " << name<decltype(eps0)::dimension>() << std::endl;

	//classical electron radius
	constexpr auto re = e*e/(4*std::numbers::pi*me*eps0);
	std::cout << "Classical radius of electron = " << re/m << " m" << std::endl;

	//Thomson cross section
	constexpr auto sigmaT  =  8*std::numbers::pi * re*re/3;
	constexpr auto bn = 1e-28*m*m;
	std::cout << "Thomson cross section  = " << sigmaT/bn << " barn" << std::endl;

	//Bohr magneton
	constexpr auto muB = e*hbar/(2*me/c/c);
	//tesla
	constexpr auto T = kg/(s*s*A);
	std::cout << "Bohr magneton = " << muB/MeV*T << "   MeV/T or " << name<decltype(muB)::dimension>() << std::endl;

}
