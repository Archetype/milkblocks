#ifndef _ANIMATABLE_H_
#define _ANIMATABLE_H_

#ifndef _MILKBLOCKS_H_
#include "milkblocks.h"
#endif

#ifndef _VECTOR_
#include "vector"
#endif

class Animatable;

class Animation
{
	void (*Complete)(Animation*, Animatable*, void*);
	double (*Easer) (double t, double d);
	int StartTime;
	double Duration;
	double Base;
	double Target;
	double* Value;
	void* UserData;

	public:
		void* Data;

		Animation( double* value, double target, double duration, void (*callback)( Animation*, Animatable*, void* ) = NULL, double (*easer) (double t, double d) = NULL, void* userdata = NULL );
		bool Update( Animatable* animatable, int t );
};

class Animatable : public std::vector<Animatable*>
{
	public:
		double x;
		double y;
		double scaleX;
		double scaleY;
		double alpha;
		const char* name;
		int ID;

		enum Flags
		{
			ALPHA = (1<<0),
			X = (1<<1),
			Y = (1<<2),
			SCALE_X = (1<<3),
			SCALE_Y = (1<<4),
			DELETE_NEXT_UPDATE = (1<<7)
		};

		int numChildren() { return size(); }

	private:
		Animation* xAnimation;
		Animation* yAnimation;
		Animation* alphaAnimation;
		Animation* scaleXAnimation;
		Animation* scaleYAnimation;
		unsigned char flags;
		Animatable* mParent;

	public:
		Animation* Animate( int type, float target, int duration,  void (*callback)( Animation*, Animatable*, void* ) = NULL, double (*easer) (double t, double d) = NULL, void* userdata = NULL );
		Animatable* Parent() { return mParent; }

		Animatable();
		virtual ~Animatable();
		void removeChild( Animatable* child );
		void addChild( Animatable* child );

		void Render( double a );
		void Advance();
		void UpdateAnimations();
		void UpdateAnimation( int time,  Animation** animation );

		bool checkDelete() { if(flags&DELETE_NEXT_UPDATE ) { return true; } else return false; }
		void deleteOnNextUpdate() { flags |= DELETE_NEXT_UPDATE; }
	protected:
		virtual void onRender();
		virtual void onAdvance();
};



#endif