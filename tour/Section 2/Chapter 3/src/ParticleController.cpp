#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "ParticleController.h"

using namespace ci;
using std::list;

ParticleController::ParticleController()
{
}

void ParticleController::applyForceToParticles( float zoneRadiusSqrd, float thresh )
{
	float twoPI = M_PI * 2.0f;
	for( list<Particle>::iterator p1 = mParticles.begin(); p1 != mParticles.end(); ++p1 ){
	
		list<Particle>::iterator p2 = p1;
		for( ++p2; p2 != mParticles.end(); ++p2 ) {
			Vec3f dir = p1->mPos - p2->mPos;
			float distSqrd = dir.lengthSquared();
			
			if( distSqrd < zoneRadiusSqrd ){	// Neighbor is in the zone
				float percent = distSqrd/zoneRadiusSqrd;
				
				if( percent < thresh ){			// Separation
					float F = ( thresh/percent - 1.0f ) * 0.01f;
					dir.normalize();
					dir *= F;
			
					p1->mAcc += dir;
					p2->mAcc -= dir;
					
				} else {						// Cohesion
					float threshDelta = 1.0f - thresh;
					float adjustedPercent = ( percent - thresh )/threshDelta;
					float F = ( 1.0 - ( cos( adjustedPercent * twoPI ) * -0.5f + 0.5f ) ) * 0.05f;
					
					// INTERESTING BUG
					// Use this F instead and lower the thresh to 0.2 after flattening the scene ('f' key)
					// float F = ( 0.5f - ( cos( adjustedPercent * twoPI ) * 0.5f + 0.5f ) ) * 0.15f;
										
					dir.normalize();
					dir *= F;
			
					p1->mAcc -= dir;
					p2->mAcc += dir;
					
				}
			}
		}
	}
}

void ParticleController::pullToCenter( const ci::Vec3f &center )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->pullToCenter( center );
	}
}

void ParticleController::update( bool flatten )
{
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->update( flatten );
	}
}

void ParticleController::draw()
{
	gl::color( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->draw();
	}
	
	glBegin( GL_LINES );
	for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->drawTail();
	}
	glEnd();
}

void ParticleController::addParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		Vec3f randVec = Rand::randVec3f();
		Vec3f pos = randVec * Rand::randFloat( 50.0f, 250.0f );
		Vec3f vel = randVec * 2.0f;
		mParticles.push_back( Particle( pos, vel ) );
	}
}


