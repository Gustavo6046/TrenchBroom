/*
Copyright (C) 2010-2012 Kristian Duske

This file is part of TrenchBroom.

TrenchBroom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TrenchBroom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
*/

#import "DragFaceToolFeedbackFigure.h"
#import "Camera.h"
#import "GLUtils.h"
#import "Brush.h"
#import "Face.h"

@implementation DragFaceToolFeedbackFigure

- (void)render {
    TVector3f mid;
    
    if ([brushes count] > 0) {
        if (vertexHandle == NULL) {
            vertexHandle = gluNewQuadric();
            gluQuadricDrawStyle(vertexHandle, GLU_FILL);
        }
        
        glFrontFace(GL_CCW);
        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_DEPTH_TEST);
        glColorV4f(&color);
        
        for (id <Brush> brush in brushes) {
            for (id <Face> face in [brush faces]) {
                centerOfVertices([face vertices], &mid);
                float dist = [camera distanceTo:&mid];
                
                glPushMatrix();
                glTranslatef(mid.x, mid.y, mid.z);
                glScalef(dist / 300, dist / 300, dist / 300);
                gluSphere(vertexHandle, radius, 12, 12);
                glPopMatrix();
            }
        }
        
        glFrontFace(GL_CW);
        glDisable(GL_DEPTH_TEST);
    }
}

- (id)initWithCamera:(Camera *)theCamera radius:(float)theRadius color:(const TVector4f *)theColor {
    NSAssert(theCamera != nil, @"camera must not be nil");
    NSAssert(theRadius > 0, @"radius must be greater than 0");
    NSAssert(theColor != NULL, @"color must not be NULL");
    
    if ((self = [self init])) {
        camera = theCamera;
        radius = theRadius;
        color = *theColor;
    }
    
    return self;
}


- (void)setBrushes:(NSArray *)theBrushes {
    NSAssert(theBrushes != nil, @"brush array must not be nil");
    brushes = theBrushes;
}

@end
