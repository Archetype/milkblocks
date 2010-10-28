#include "Animatable.h"
#include "Easing/Linear.h"

static int idx = 0;
Animation::Animation( double* value, double target, double duration, void (*callback)( Animation*, Animatable*, void* ), double (*easer) (double t, double d), void* userdata )
{
	StartTime = glutGet( GLUT_ELAPSED_TIME );
	Value = value;
	Base = *Value;
	Target = target;
	Duration = duration;
	Complete = callback;
	Easer = easer;
	UserData = userdata;
}

bool Animation::Update( Animatable* animatable, int t )
{
	t -= StartTime;

	double p = 1;
	if( t < Duration )
		p = Easer( (double)t, Duration ); 
	
	*Value = Base*(1.0 - p) + Target*p;

	if( p == 1 )
	{
		if( Complete != NULL )
			Complete( this, animatable, UserData );
		return true;
	}
	return false;
}

Animation* Animatable::Animate( int type, float target, int duration,  void (*callback)( Animation*, Animatable*, void* ), double (*easer) (double t, double d), void* userdata )
{
	double* value;
	Animation** tracker;
	Animation* delAnimation = NULL;
	switch( type )
	{
		case ALPHA:
			tracker = &alphaAnimation;
			value = &alpha;
			break;
		case X:
			tracker = &xAnimation;
			value = &x;
			break;
		case Y:
			tracker = &yAnimation;
			value = &y;
			break;
		case SCALE_X:
			tracker = &scaleXAnimation;
			value = &scaleX;
			break;
		case SCALE_Y:
			tracker = &scaleYAnimation;
			value = &scaleY;
			break;
	}
	if( *tracker != NULL )
		delAnimation = *tracker;
	
	*tracker = new Animation( value, target, duration, callback, easer == NULL ? Linear::easeNone : easer, userdata);
	delete delAnimation; // we delete the original animation after we allocate the new one!!
	return *tracker;  
}

Animatable::Animatable() : x(0),y(0),alpha(1),xAnimation(NULL),yAnimation(NULL),alphaAnimation(NULL), scaleXAnimation(NULL), scaleYAnimation(NULL), scaleX(1.0), scaleY(1.0), flags(0), mParent( NULL )
{
	name = "Animatable";
	ID = idx++;
}

Animatable::~Animatable()
{
	delete alphaAnimation;
	delete xAnimation;
	delete yAnimation;
}

void Animatable::UpdateAnimation( int time,  Animation** animation )
{
	Animation* pAnim = *animation;
	// check animation isn't null, see if it completed, make sure another one didn't take its place
	if( pAnim != NULL && pAnim->Update( this, time ) && pAnim == *animation )
	{
		delete pAnim;
		*animation = NULL;
	}
}

void Animatable::UpdateAnimations()
{
	int time = glutGet( GLUT_ELAPSED_TIME );
	UpdateAnimation( time, &alphaAnimation );
	UpdateAnimation( time, &xAnimation );
	UpdateAnimation( time, &yAnimation );
	UpdateAnimation( time, &scaleXAnimation );
	UpdateAnimation( time, &scaleYAnimation );/*
	if( alphaAnimation != NULL && alphaAnimation->Update( this, time ) )
	{
		delete alphaAnimation;
		alphaAnimation = NULL;
	}
	if( xAnimation != NULL && xAnimation->Update( this, time ) )
	{
		delete xAnimation;
		xAnimation = NULL;
	}
	if( yAnimation != NULL && yAnimation->Update( this, time ) )
	{
		delete yAnimation;
		yAnimation = NULL;
	}*/
}

void Animatable::addChild( Animatable* child )
{
	if( child->mParent != NULL )
		child->mParent->removeChild( child );

	push_back( child );
	child->mParent = this;
}

void Animatable::removeChild( Animatable* child )
{
	for( iterator i = begin(); i != end(); i++ )
	{
		if( (*i) == child )
		{		
			erase( i );
			break;
		}
	}
}

void Animatable::Render( double a )
{
	//printf( "%s %i\n", name, ID );
	a *= alpha;
	glPushMatrix();
	glTranslated( x, y, 0 );
	glScaled( scaleX, scaleY, 1.0 );
	glColor4d( 1.0, 1.0, 1.0, a );
	onRender();
	for( iterator i = begin(); i != end(); i++ )
	{
		(*i)->Render( a );
	}
	glPopMatrix();
}

void Animatable::onRender()
{
}

void Animatable::Advance()
{
	onAdvance();
	for( int i = 0; i < size(); i++ )
	{
		Animatable* child = (*this)[i];
		
		if( child->checkDelete() )
		{
			erase( begin()+i );
			delete child;
			i--;
			continue;
		}

		child->UpdateAnimations();
		if( i < size() )
		{
			if( (*this)[i] == child )
				child->Advance();
			else // deleted, process it again...
				i--;
		}
	}
}

void Animatable::onAdvance()
{
}