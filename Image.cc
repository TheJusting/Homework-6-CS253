#include "PGM.h"
#include "Image.h"
#include "Alpha.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

Image* Image::create(const string filename){
        Image* im;
        try{
                ifstream in;
                string name;
                in.open(filename);
                if(in.fail()) throw string("Bad File: file " + filename + " does not exist.");
                else{
                        in >> name;
                        if(name == "P2") im = new PGM(filename);
                        else if(name == "Alpha") im = new Alpha(filename);
                        else throw string("Bad File: Expected \"Alpha\" or \"P2\", got " + name);
                }
        }catch(string Err){
                cerr << Err << "\n";
        }
        return im;
}

Image::Image() : h(-1), w(-1), maxp(-1){}

Image::Image(const Image &a){
        Image::v = a.v;
        Image::w = a.w;
        Image::h = a.h;
        Image::maxp = a.maxp;
        Image::image = a.image;
}

Image &Image::operator=(const Image &a){
        Image::v = a.v;
        Image::image = a.image;
        Image::maxp = a.maxp;
        Image::h = a.h;
        Image::w = a.w;
        return *this;
}

Image::~Image(){}

void Image::min(const Image &a){
	Image::min(Image::NW, a);
}

void Image::min(string direction, const Image &a){
	unsigned int col, row;
	int end;
	if(a.width() < Image::width()) col = a.width();
	else col = Image::width();
	if(a.height() < Image::height()) row = a.height();
	else row = Image::height();
	switch(direction){
		case "NW":
			for(auto i = 0; i < row; i++){
				for(auto j = 0; j < col; j++){
					Image::minHelper(a, j, i, j, i);
				}
			}
			break;
		case "NE":
			for(auto i = 0; i < row; i++){
				for(auto j = 0; j < col; j++){
					Image::minHelper(a, Image::width-(1+j), i, a.width()-(1+j), i);
				}
			}
			break;
		case "SW":
			for(auto i = 0; i < row; i++){
				for(auto j = 0; j < col; j++){
					Image::minHelper(a, j,Image::height()-(1+i), j, a.height()-(1+i)); 
				}
			}
			break;
		case "SE":
			for(auto i = 0; i < row; i++){
				for(auto j = 0; j < col; j++){
					Image::minHelper(a, Image::width-(1+j), Image::height()-(1+i), a.width()-(1+k), a.height()-(1+i)); 
				}
			}
			break;
		default:
			throw string("Invalid direction declaration " + direction);
			break;
	}
}


void minHelper(const Image &a, int j, int i, int j1, int i1){
	double lhs, rhs;
	bool lScaled = false;
	if(a.range() < Image::range()){
		rhs = Image::scale(a.get(j1, i1), a.range(), Image::range());
		lhs = Image::get(j, i);
	}else if(Image::range() < a.range()){
		lhs = Image::scale(Image::get(j, i), Image::range(), a.range());
		rhs = a.get(j1, i1);
		lScaled = true;
	}else{
		lhs = Image::get(j, i);
		rhs = a.get(j1, i1);
	}
	if(rhs < lhs){
		if(lScaled){
				rhs = Image::scale(rhs, a.range(), Image::range());
				v[i][j] = rhs;
		}else v[i][j] = rhs;
	}
}

int scale(int val, int range1, int range2){
        return (val / (double)range1) * range2;
}

bool Image::empty() const{
        if(Image::height() == 0 && Image::width() == 0) return true;
        else return false;
}

int Image::height() const{
        return Image::h;
}

int Image::width() const{
        return Image::w;
}

int Image::range() const{
        return Image::maxp;
}

void Image::mirror(){
    for(unsigned int i = 0; i < Image::v.size(); i++){
        for(unsigned int j = 0; j < (Image::v[i].size() / 2); j++){
            int c = Image::v[i][j];
            Image::v[i][j] = Image::v[i][Image::v[i].size()- j - 1];
            Image::v[i][Image::v[i].size()-j-1] = c;
        }
    }
}

void Image::rotate(int degrees){
        if(degrees % 90 != 0) throw string("Invalid degrees of rotation");
        else if(degrees == 0 || degrees % 360 == 0) return;
                else if(degrees % 270 == 0) degrees = 3;
        else if(degrees % 180 == 0) degrees = 2;
        else degrees = 1;
        while(degrees > 0){
                vector<vector<int>> v2;
                                vector<int> temp;
                for(unsigned int cols = 0; cols < Image::v[0].size(); cols++){
                        for(int rows = Image::v.size() - 1; rows >= 0; rows--){
                                temp.push_back(Image::v[rows][cols]);
                        }
                                                v2.push_back(temp);
                }
                Image::v = v2;
                                int swap = Image::h;
                                Image::h = Image::w;
                                Image::w = swap;
                degrees--;
        }
}

void Image::resize(double factor){
        if(factor == 2){
                        for(unsigned int i = 0; i < Image::v.size(); i += 2){
                                for(unsigned int j = 0; j < Image::v[i].size(); j += 2){
                                                Image::v[i].insert(Image::v[i].begin()+(j+1), Image::v[i][j]);
                                }
                                vector<int> temp(Image::v[i]);
                                Image::v.insert(Image::v.begin()+(i+1), temp);
                        }
                        Image::h *= 2;
                        Image::w *= 2;
        }
        else if(factor == 0.5){
                        vector<vector<int>> v2;
                        for(unsigned int i = 1; i < Image::v.size(); i+=2){
                                        vector<int> temp;
                                        for(unsigned int j = 1; j < Image::v[i].size(); j+=2){
                                                        int avg = (Image::v[i][j] + Image::v[i-1][j] + Image::v[i][j-1] + Image::v[i-1][j-1])/4;
                                                        temp.push_back(avg);
                                        }
                                        v2.push_back(temp);
                        }
                        Image::v = v2;
                        Image::h /= 2;
                        Image::w /= 2;
        }
        else throw string("Invalid factor for resize");
}