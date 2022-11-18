#include <iostream>
#include <stdlib.h>

struct Point_t {
	float x_ = NAN, y_ = NAN, z_ = NAN;

	Point_t(float x, float y, float z) : x_(x), y_(y), z_(z) {}
	Point_t() {}

	//void Point_t_redefine(float x, float y, float z);

	void print() const;
	bool valid() const;
	bool equal(const Point_t &that) const;
};



//  x-x0 / m = y - y0 / n = z - z0 / p
struct Line_t {
	float x0_ = 0.0f, y0_ = 0.0f, z0_ = 0.0f;
	float m_ = 0.0f, n_ = 0.0f, p_ = 0.0f;
	void print() const;
	bool valid() const;

	Line_t(const Point_t &p1, const Point_t &p2);

	bool valid() const;
	bool parallel() const;
 	//Line_t() {}
}  


//  ax + by + cz + d = 0
struct Plane_t
{  
	float a_ = 0.0, b_ = 0.0, c_ = 0.0, d_ = 0.0;
	void print() const;
	bool valid() const;

	Plane_t(const Point_t &p1, const Point_t &p2, const Point_t &p3);
	//Plane_t() {}

	void Plane_t_redefine(const Point_t &p1, const Point_t &p2, const Point_t &p3);
	//Plane_t(const Line_t &l1, const Line_t &l2);

	Line_t Crossing_by_plane(const Plane_t &that);
	Point_t Crossing_by_line(const Line_t &that);


};




struct Triangle_t
{
	Point_t A, B, C;
	Plane_t main_plain_;
	Line_t AB, BC, AC;
	bool cross_with_other = 0;

	Triangle_t(const Point_t &p1, const Point_t &p2, const Point_t &p3);

	Triangle_t();

};


Triangle_t::Triangle_t(){

	float dots_for_point[3] = {};

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++i)
		{
			std::cin >> dots_for_point[j];
		}
		A = 
	}


}


Line_t::Line_t (const Point_t &p1, const Point_t &p2) {

	if( p1.valid && p2.valid && !p1.equal(p2)){
		x0_ = p1.x_;
		y0_ = p1.y_;
		z0_ = p1.z_;
		m_ = p2.x_ - p1.x_;
		n_ = p2.y_ - p1.y_;
		p_ = p2.z_ - p1.z_;
	}

}


Plane_t::Plane_t(const Point_t &p0, const Point_t &p1, const Point_t &p2){

	if( p0.valid && p1.valid && p2.valid && (!p0.equal(p1) || !p0.equal(p2))){
		a_ = (p1.y_ - p0.y_)*(p2.z_-p0.z_) - (p1.z_ - p0.z_)*(p2.y_-p0.y_);
		b_ = (p1.z_ - p0.z_)*(p2.x_-p0.x_) - (p1.x_ - p0.x_)*(p2.z_-p0.z_);
		z_ = (p1.x_ - p0.x_)*(p2.y_-p0.y_) - (p1.y_ - p0.y_)*(p2.x_-p0.x_);

		d_ = (p1.y_ - p0.y_)*(p0.z_ * p2.x_ - p2.z_ * p0.x_) +
			 (p1.x_ - p0.x_)*(p0.x_ * p2.y_ - p2.x_ * p0.y_) +
			 (p1.z_ - p0.z_)*(p0.y_ * p2.z_ - p2.y_ * p0.z_);
	}

}







