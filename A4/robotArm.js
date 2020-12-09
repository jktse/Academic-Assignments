"use strict";

var canvas, gl, program;

var NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

var points = [];
var colors = [];
var cubeNormal = [[ 0, 0, 1],
  [ 0, 0, 1],
  [ 0, 0, 1],
  [ 0, 0, 1],
  [ 0, 0, 1],
  [ 0, 0, 1],
  [ 1, 0, 0],
  [ 1, 0, 0],
  [ 1, 0, 0],
  [ 1, 0, 0],
  [ 1, 0, 0],
  [ 1, 0, 0],
  [ 0, -1, 0],
  [ 0, -1, 0],
  [ 0, -1, 0],
  [ 0, -1, 0],
  [ 0, -1, 0],
  [ 0, -1, 0],
  [ 0, 1, 0],
  [ 0, 1, 0],
  [ 0, 1, 0],
  [ 0, 1, 0],
  [ 0, 1, 0],
  [ 0, 1, 0],
  [ 0, 0, -1],
  [ 0, 0, -1],
  [ 0, 0, -1],
  [ 0, 0, -1],
  [ 0, 0, -1],
  [ 0, 0, -1],
  [ -1 , 0, 0],
  [ -1 , 0, 0],
  [ -1 , 0, 0],
  [ -1 , 0, 0],
  [ -1 , 0, 0],
  [ -1 , 0, 0]];

var topView = false;
var fetchItem = false;
var hasItem = false;
var rest = false;

var vertices = [
    vec4( -0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5,  0.5,  0.5, 1.0 ),
    vec4(  0.5, -0.5,  0.5, 1.0 ),
    vec4( -0.5, -0.5, -0.5, 1.0 ),
    vec4( -0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5,  0.5, -0.5, 1.0 ),
    vec4(  0.5, -0.5, -0.5, 1.0 )
];

// RGBA colors
var vertexColors = [
    vec4( 0.0, 0.0, 0.0, 1.0 ),  // black
    vec4( 1.0, 0.0, 0.0, 1.0 ),  // red
    vec4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    vec4( 0.0, 1.0, 0.0, 1.0 ),  // green
    vec4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    vec4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    vec4( 0.0, 1.0, 1.0, 1.0 ),  // white
    vec4( 0.0, 1.0, 1.0, 1.0 )   // cyan
];

// Cylinder Data code from https://www.cs.unm.edu/~ANGEL/WebGL/CODE/GEOMETRY/cylinderData.js
var cylinderData = cylinder(288,20,true);
var cylinderVerts = cylinderData.TriangleVertices;
var cylinderColor = cylinderData.TriangleVertexColors;
var cylinderNormals = cylinderData.TriangleNormals;

// Sphere Data code from https://www.cs.unm.edu/~ANGEL/WebGL/CODE/GEOMETRY/sphereData.js
var sphereData = sphere(6);
var sphereVerts = sphereData.TriangleVertices;
var sphereColor = sphereData.TriangleVertexColors;
var sphereNormals = sphereData.TriangleNormals;

// Parameters controlling the size of the Robot's arm

var BASE_HEIGHT      = 0.5;
var BASE_WIDTH       = 1.0;
var LOWER_ARM_HEIGHT = 2.0;
var LOWER_ARM_WIDTH  = 0.3;
var UPPER_ARM_HEIGHT = 2.0;
var UPPER_ARM_WIDTH  = 0.3;
// Parameters for the sphere
var SPHERE_RADIUS    = 0.15;

// Parameters for where the sphere initial position should be drawn.
var oldSpherePos = vec3(0.15, 0.15, 0.15);
var newSpherePos = vec3(0.0, 0.0, 0.0);

// Shader transformation matrices

var modelMatrix, viewMatrix, projectionMatrix, lookMatrix;

// Buffers
var vColor, vPosition, vNormal;

// Array of rotation angles (in degrees) for each rotation axis

var Base = 0;
var LowerArm = 1;
var UpperArm = 2;


var theta = [ 0, 0, 0];
var goalTheta = [0, 0, 0];

var angle = 0;

var modelMatrixLoc, viewMatrixLoc, projectionMatrixLoc, normalMatrixLoc;
var KaLoc, KsLoc, KdLoc;
var shininessValLoc;
var ambientColorLoc, specularColorLoc, lightPositionLoc;

var cylinderVBuffer, cylinderCBuffer, cylinderNBuffer;
var sphereVBuffer, sphereCBuffer, sphereNBuffer;
var cubeVBuffer, cubeCBuffer, cubeNBuffer;

//For Phong Illumination
var KaVal = 1.0;
var KdVal = 0.85;
var KsVal = 1.0;
var shinyVal = 25.0;
var lightPosition = vec3(0.0, 2.0, 3.0);
var ambientColor = vec4(0.0, 0.0, 0.0, 1.0);
var specularColor = vec4(1.0, 1.0, 1.0, 1.0);

//----------------------------------------------------------------------------

function quad(  a,  b,  c,  d ) {
    colors.push(vertexColors[a]);
    points.push(vertices[a]);
    colors.push(vertexColors[a]);
    points.push(vertices[b]);
    colors.push(vertexColors[a]);
    points.push(vertices[c]);
    colors.push(vertexColors[a]);
    points.push(vertices[a]);
    colors.push(vertexColors[a]);
    points.push(vertices[c]);
    colors.push(vertexColors[a]);
    points.push(vertices[d]);
}


function colorCube() {
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//____________________________________________

// Remmove when scale in MV.js supports scale matrices

function scale4(a, b, c) {
   var result = mat4();
   result[0][0] = a;
   result[1][1] = b;
   result[2][2] = c;
   return result;
}


//--------------------------------------------------


window.onload = function init() {

    canvas = document.getElementById( "gl-canvas" );

    gl = WebGLUtils.setupWebGL( canvas );
    if ( !gl ) { alert( "WebGL isn't available" ); }

    gl.viewport( 0, 0, canvas.width, canvas.height );

    gl.clearColor( 1.0, 1.0, 1.0, 1.0 );
    gl.enable( gl.DEPTH_TEST );

    colorCube();
    // Load shaders and use the resulting shader program

    program = initShaders( gl, "vertex-shader", "fragment-shader" );
    gl.useProgram( program );

    // Create and initialize  buffer objects

    // Cube Buffer
    cubeVBuffer = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, cubeVBuffer );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(points), gl.STATIC_DRAW );

    cubeCBuffer = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, cubeCBuffer );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(colors), gl.STATIC_DRAW );

    cubeNBuffer = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, cubeNBuffer );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(cubeNormal), gl.STATIC_DRAW );

    // Cylinder Buffer
    cylinderVBuffer = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, cylinderVBuffer );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(cylinderVerts), gl.STATIC_DRAW );

    cylinderCBuffer = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, cylinderCBuffer );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(cylinderColor), gl.STATIC_DRAW );

    cylinderNBuffer = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, cylinderNBuffer );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(cylinderNormals), gl.STATIC_DRAW );

    // Shpere Buffer
    sphereVBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, sphereVBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(sphereVerts), gl.STATIC_DRAW);

    sphereCBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, sphereCBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(sphereColor), gl.STATIC_DRAW);

    sphereNBuffer = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, sphereNBuffer );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(sphereNormals), gl.STATIC_DRAW );

    vColor = gl.getAttribLocation( program, "vColor" );
    gl.enableVertexAttribArray( vColor );

    vPosition = gl.getAttribLocation( program, "vPosition" );
    gl.enableVertexAttribArray( vPosition );

    vNormal = gl.getAttribLocation(program, "vNormal");
    gl.enableVertexAttribArray(vNormal);

    // Get and store Uniform Locations
    modelMatrixLoc = gl.getUniformLocation(program, "modelMatrix");
    viewMatrixLoc = gl.getUniformLocation(program, "viewMatrix");
    projectionMatrixLoc = gl.getUniformLocation(program, "projectionMatrix");
    normalMatrixLoc = gl.getUniformLocation(program, "normalMatrix");

    KaLoc = gl.getUniformLocation(program, "Ka");
    KdLoc = gl.getUniformLocation(program, "Kd");
    KsLoc = gl.getUniformLocation(program, "Ks");

    shininessValLoc = gl.getUniformLocation(program, "shininessVal");

    ambientColorLoc = gl.getUniformLocation(program, "ambientColor");
    specularColorLoc = gl.getUniformLocation(program, "specularColor");
    lightPositionLoc = gl.getUniformLocation(program, "lightPosition")

    // This matrix is constant
    projectionMatrix = ortho(-7, 7, -7, 7, -7, 7);

    render();
}


//----------------------------------------------------------------------------


function base() {
    var s = scale4(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH);
    var instanceMatrix = mult( translate( 0.0, 0.5 * BASE_HEIGHT, 0.0 ), s);
    var t = mult(modelMatrix, instanceMatrix);
    gl.uniformMatrix4fv(modelMatrixLoc,  false, flatten(t));
    
    //Compute the normal Matrix
    var normMatrix = normalMatrix(mult(viewMatrix, t));
    gl.uniformMatrix4fv(normalMatrixLoc, false, flatten(normMatrix));

    gl.drawArrays( gl.TRIANGLES, 0, cylinderVerts.length );
}

//----------------------------------------------------------------------------


function upperArm() {
    var s = scale4(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
    var instanceMatrix = mult(translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ),s);
    var t = mult(modelMatrix, instanceMatrix);
    gl.uniformMatrix4fv( modelMatrixLoc,  false, flatten(t) );

    //Compute the normal Matrix
    var normMatrix = normalMatrix(mult(viewMatrix, t));
    gl.uniformMatrix4fv(normalMatrixLoc, false, flatten(normMatrix));

    gl.drawArrays( gl.TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------


function lowerArm()
{
    var s = scale4(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
    var instanceMatrix = mult( translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ), s);
    var t = mult(modelMatrix, instanceMatrix);
    gl.uniformMatrix4fv( modelMatrixLoc,  false, flatten(t) );
    
    //Compute the normal Matrix
    var normMatrix = normalMatrix(mult(viewMatrix, t));
    gl.uniformMatrix4fv(normalMatrixLoc, false, flatten(normMatrix));

    gl.drawArrays( gl.TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------


function drawHand(){
    var s = scale4(SPHERE_RADIUS,SPHERE_RADIUS,SPHERE_RADIUS);
    var instanceMatrix = mult(translate( 0.0, UPPER_ARM_HEIGHT, 0.0 ),s);
    var t = mult(modelMatrix, instanceMatrix);
    gl.uniformMatrix4fv(modelMatrixLoc, false, flatten(t));
    
    //Compute the normal Matrix
    var normMatrix = normalMatrix(mult(viewMatrix, t));
    gl.uniformMatrix4fv(normalMatrixLoc, false, flatten(normMatrix));

    gl.drawArrays(gl.TRIANGLES, 0, sphereVerts.length);
}


//----------------------------------------------------------------------------


function drawSphere(){
    var s = scale4(SPHERE_RADIUS,SPHERE_RADIUS,SPHERE_RADIUS);
    if(hasItem){
        var instanceMatrix = mult(translate( 0.0, UPPER_ARM_HEIGHT, 0.0 ),s);
    }else{
        var instanceMatrix = mult(translate(oldSpherePos[0], oldSpherePos[1], oldSpherePos[2]), s);
    }
    var t = mult(modelMatrix, instanceMatrix);
    gl.uniformMatrix4fv(modelMatrixLoc, false, flatten(t));

    //Compute the normal Matrix
    var normMatrix = normalMatrix(mult(viewMatrix, t));
    gl.uniformMatrix4fv(normalMatrixLoc, false, flatten(normMatrix));

    gl.drawArrays(gl.TRIANGLES, 0, sphereVerts.length);
}


//----------------------------------------------------------------------------


var updateAttributesAndUniforms = function(){
    // Fill in the uniform Variables.
    gl.uniform1f(KaLoc, KaVal);
    gl.uniform1f(KsLoc, KsVal);
    gl.uniform1f(KdLoc, KdVal);
    gl.uniform1f(shininessValLoc, shinyVal);

    gl.uniform3f(ambientColorLoc, ambientColor[0], ambientColor[1], ambientColor[2]);
    gl.uniform3f(specularColorLoc, specularColor[0], specularColor[1], specularColor[2]);
    gl.uniform3f(lightPositionLoc, lightPosition[0], lightPosition[1], lightPosition[2]);

    gl.uniformMatrix4fv(projectionMatrixLoc, false, flatten(projectionMatrix));
    gl.uniformMatrix4fv(viewMatrixLoc, false, flatten(viewMatrix));
}


//----------------------------------------------------------------------------


var render = function() {
    gl.clearColor(0.75, 0.85, 0.8, 1.0);
    gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT );

    if(topView){
        viewMatrix = lookAt([0,4,0], [0,-5,0], [0,0,1]);
    }else{
        viewMatrix = lookAt([0,0.15,1], [0,0,0], [0,1,0]);
    }
    
    updateAttributesAndUniforms();

    modelMatrix = rotate(theta[Base], 0, 1, 0 );
    gl.bindBuffer( gl.ARRAY_BUFFER, cylinderVBuffer );
    gl.vertexAttribPointer(vPosition, 4, gl.FLOAT, gl.FALSE, 0, 0);

    gl.bindBuffer( gl.ARRAY_BUFFER, cylinderNBuffer );
    gl.vertexAttribPointer(vNormal, 3, gl.FLOAT, gl.FALSE, 0, 0);

    gl.bindBuffer( gl.ARRAY_BUFFER, cylinderCBuffer );
    gl.vertexAttribPointer(vColor, 4, gl.FLOAT, gl.FALSE, 0, 0);
    base();

    gl.bindBuffer( gl.ARRAY_BUFFER, cubeVBuffer );
    gl.vertexAttribPointer(vPosition, 4, gl.FLOAT, gl.FALSE, 0, 0);

    gl.bindBuffer( gl.ARRAY_BUFFER, cubeNBuffer );
    gl.vertexAttribPointer(vNormal, 3, gl.FLOAT, gl.FALSE, 0, 0);

    gl.bindBuffer( gl.ARRAY_BUFFER, cubeCBuffer );
    gl.vertexAttribPointer(vColor, 4, gl.FLOAT, gl.FALSE, 0, 0);

    modelMatrix = mult(modelMatrix, translate(0.0, BASE_HEIGHT, 0.0));
    modelMatrix = mult(modelMatrix, rotate(theta[LowerArm], 0, 0, 1 ));
    lowerArm();

    modelMatrix  = mult(modelMatrix, translate(0.0, LOWER_ARM_HEIGHT, 0.0));
    modelMatrix  = mult(modelMatrix, rotate(theta[UpperArm], 0, 0, 1) );
    upperArm();

    drawHand();

    // If fetch true we need update the angle of the robotic arm
    if(fetchItem){
        updateAngles(true);
    }


    if(theta[Base] != goalTheta[Base] | theta[LowerArm] != goalTheta[LowerArm] | theta[UpperArm] != goalTheta[UpperArm]){
        //console.log("Hi");
        for(var i = 0; i < 3; i ++){
            if(theta[i] != goalTheta[i]){
                if(theta[i] > goalTheta[i]){
                    //Case if original angle is > than goal
                    if(theta[i] - goalTheta[i] < 0.25){
                        theta[i] = theta[i] - (theta[i] - goalTheta[i]);
                    }else{
                        theta[i] = theta[i] - 0.25;
                    }
                }else{
                    //Case if original angle < than goal
                    if(goalTheta[i] - theta[i] < 0.25){
                        theta[i] = theta[i] + (goalTheta[i] - theta[i]);
                    }else{
                        theta[i] = theta[i] + 0.25;
                    }
                }
           }
        }
        if((theta[Base] == goalTheta[Base] && theta[LowerArm] == goalTheta[LowerArm] && theta[UpperArm] == goalTheta[UpperArm]) && !hasItem && !rest){
            //console.log("Got Item");
            hasItem = true;
            updateAngles(false);
            //console.log(goalTheta);
        }else if((theta[Base] == goalTheta[Base] && theta[LowerArm] == goalTheta[LowerArm] && theta[UpperArm] == goalTheta[UpperArm]) && hasItem && !rest){
            //console.log("Rest");
            hasItem = false;
            rest = true;
            goalTheta[Base] = 0;
            goalTheta[LowerArm] = 0;
            goalTheta[UpperArm] = 0;
        }
    }

    if((theta[Base] == goalTheta[Base] && theta[LowerArm] == goalTheta[LowerArm] && theta[UpperArm] == goalTheta[UpperArm]) && hasItem && !rest){
        //console.log("Rest");
        hasItem = false;
        rest = true;
        goalTheta[Base] = 0;
        goalTheta[LowerArm] = 0;
        goalTheta[UpperArm] = 0;
    }


    gl.bindBuffer( gl.ARRAY_BUFFER, sphereVBuffer );
    gl.vertexAttribPointer(vPosition, 4, gl.FLOAT, gl.FALSE, 0, 0);

    gl.bindBuffer( gl.ARRAY_BUFFER, sphereNBuffer );
    gl.vertexAttribPointer(vNormal, 3, gl.FLOAT, gl.FALSE, 0, 0);

    gl.bindBuffer( gl.ARRAY_BUFFER, sphereCBuffer );
    gl.vertexAttribPointer(vColor, 4, gl.FLOAT, gl.FALSE, 0, 0);

    if(!hasItem){
    modelMatrix = mat4();
    }
    drawSphere();
    requestAnimFrame(render);
}


var updateAngles = function(isOld){
    if(isOld){
        var deltaX = oldSpherePos[0];
        var deltaY = oldSpherePos[1];
        var deltaZ = oldSpherePos[2];
    }else{
        oldSpherePos[0] = newSpherePos[0];
        oldSpherePos[1] = newSpherePos[1];
        oldSpherePos[2] = newSpherePos[2];
        var deltaX = newSpherePos[0];
        var deltaY = newSpherePos[1];
        var deltaZ = newSpherePos[2];
    }
    
    var length = 2.0;
    var distanceSphere = distanceVector([deltaX, deltaY, deltaZ], [0, 0.5, 0]);

    /** Problem with the 3D portion of doing it. IF z = 0.0 we have problem **/
    // Rotate so so we are on the same plane for rotations of the arm.
    var rotateY = Math.atan(deltaZ/deltaX);

    if(deltaX < 0){
        goalTheta[Base] = -(rotateY * 180.0/Math.PI) + 180;
    }else{
        goalTheta[Base] = -(rotateY * 180.0/Math.PI);
    }
    

    // Angle for lower arm
    var alpha = Math.acos(((distanceSphere*distanceSphere) + (length * length) - (length * length))/(2 * distanceSphere * length));
    
    var aPrime = Math.atan((deltaY - 0.5)/(Math.sqrt(deltaX * deltaX + deltaZ * deltaZ)));

    goalTheta[LowerArm] = -90 + ((aPrime * 180.0/Math.PI) - (alpha * 180.0 / Math.PI));

    // Angle for upper arm
    var beta = Math.acos(((length*length) + (length * length) - (distanceSphere * distanceSphere))/(2 * length * length));
    
    goalTheta[UpperArm] = 180.0 - (beta * 180.0/Math.PI);

    //console.log(theta);

    if(isNaN(alpha) || isNaN(beta) || isNaN(aPrime)){
        if(hasItem){
            alert("New Position is unreachable, please re-enter another set of values. Arm has been reset")
        }else{
            alert("Old Position is unreachable, please re-enter another set of values. Arm has been reset")
        }
        fetchItem = false;
        rest = true;
        goalTheta[Base] = 0;
        goalTheta[LowerArm] = 0;
        goalTheta[UpperArm] = 0;
    }else{
        fetchItem = false;
        rest = false;
    }
}


var changeView = function(){
    if(topView){
        topView = false;;
    }else{
        topView = true;
    }
}

var fetch = function(){
    if(document.getElementById("old_x").value && document.getElementById("old_y").value && document.getElementById("old_z").value && document.getElementById("new_x").value && document.getElementById("new_y").value && document.getElementById("new_z").value){
        fetchItem = true;

        oldSpherePos[0] = parseFloat(document.getElementById("old_x").value);
        oldSpherePos[1] = document.getElementById("old_y").value;
        oldSpherePos[2] = document.getElementById("old_z").value;

        newSpherePos[0] = document.getElementById("new_x").value;
        newSpherePos[1] = document.getElementById("new_y").value;
        newSpherePos[2] = document.getElementById("new_z").value;
    }else{
        alert("Please fill in all values");
        console.log("No Valid Inputs");
    }
}


function distanceVector( v1, v2 )
{
    var dx = v1[0] - v2[0];
    var dy = v1[1] - v2[1];
    var dz = v1[2] - v2[2];

    return Math.sqrt( dx * dx + dy * dy + dz * dz);
}


