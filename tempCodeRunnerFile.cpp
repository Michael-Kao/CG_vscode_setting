    glBegin(GL_TRIANGLES);
    glVertex3f(eye[0], eye[1], eye[2]);
    glVertex3f(fleft, ftop, eye[2] - my_far);
    glVertex3f(fright, ftop, eye[2] - my_far);
    glEnd();