"use strict";

var canvas, gl, program;

var NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

var points = [];
var colors = [];
var normPoints = [];

var topView = false;

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

var cubeFaceNormals = [
  [ 0, 0, 1],
  [ 1, 0, 0],
  [ 0, -1, 0],
  [ 0, 1, 0],
  [ 0, 0, -1],
  [ -1 , 0, 0]
];

var cubeNormalElements = [
  0, 0, 0,
  0, 0, 0,
  1, 1, 1,
  1, 1, 1,
  2, 2, 2,
  2, 2, 2,
  3, 3, 3,
  3, 3, 3,
  4, 4, 4,
  4, 4, 4,
  5, 5, 5,
  5, 5, 5

];

var cubeElements = [
    1, 0, 3,
    3, 2, 1,

    2, 3, 7,
    7, 6, 2,

    3, 0, 4,
    4, 7, 3,

    6, 5, 1,
    1, 2, 6,

    4, 5, 6,
    6, 7, 4,

    5, 4, 0,
    0, 1, 5
];


// RGBA colors
var vertexColors = [
    vec4( 0.0, 0.0, 0.0, 1.0 ),  // black
    vec4( 1.0, 0.0, 0.0, 1.0 ),  // red
    vec4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    vec4( 0.0, 1.0, 0.0, 1.0 ),  // green
    vec4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    vec4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    vec4( 0.5, 0.5, 0.5, 1.0 ),  // white
    vec4( 0.0, 1.0, 1.0, 1.0 )   // cyan
];

var cylinderData = cylinder(72, 10, true);
var cylinderPoints = cylinderData.TriangleVertices;
var cylinderColor = cylinderData.TriangleVertexColors;
var cylinderNormal = cylinderData.TriangleNormals;

// Parameters controlling the size of the Robot's arm

var BASE_HEIGHT      = 2.0;
var BASE_WIDTH       = 2.0;
var LOWER_ARM_HEIGHT = 2.0;
var LOWER_ARM_WIDTH  = 0.3;
var UPPER_ARM_HEIGHT = 2.0;
var UPPER_ARM_WIDTH  = 0.3;

// Shader transformation matrices

var modelViewMatrix, projectionMatrix, normMatrix;

// Array of rotation angles (in degrees) for each rotation axis

var Base = 0;
var LowerArm = 1;
var UpperArm = 2;


var theta= [ 0, 0, 0];

var angle = 0;

var modelViewMatrixLoc;
var normMatrixLoc;
var diffuseColorLoc;
var lightPositionLoc;

var vCylinderBuffer, nCylinderBuffer, cCylinderBuffer;
var vCylinderLoc, nCylinderLoc, cCylinderLoc;

//Used for PhongShading
var KaVal = 0.5;
var KdVal = 1.0;
var KsVal = 1.0;
var shininess = 1.0;
var lightPosition = [1.0, 1.0, 5.0];
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

    //
    //  Load shaders and initialize attribute buffers
    //
    program = initShaders( gl, "vertex-shader", "fragment-shader" );

    gl.useProgram( program );

    colorCube();

    var i;
    for ( var i = 0; i < cubeElements.length; i++ ) {
        normPoints.push(cubeFaceNormals[cubeNormalElements[i]]);
    }
    console.log(normPoints);

    // Load shaders and use the resulting shader program

    program = initShaders( gl, "vertex-shader", "fragment-shader" );
    gl.useProgram( program );

    // Create and intialize buffers

    // Cylinders

    // Vertices
    vCylinderBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vCylinderBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(points), gl.STATIC_DRAW);
    vCylinderLoc = gl.getAttribLocation(program, "vPosition")

    // Normals
    nCylinderBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, nCylinderBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(normPoints), gl.STATIC_DRAW);
    nCylinderLoc = gl.getAttribLocation(program, "normal")

    // Colors
    cCylinderBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, cCylinderBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(colors), gl.STATIC_DRAW);
    cCylinderLoc = gl.getAttribLocation(program, "vColor")



    document.getElementById("slider1").onchange = function(event) {
        theta[0] = event.target.value;
    };
    document.getElementById("slider2").onchange = function(event) {
         theta[1] = event.target.value;
    };
    document.getElementById("slider3").onchange = function(event) {
         theta[2] =  event.target.value;
    };

    document.getElementById("lightx").onchange = function(event) {
        lightPosition[0] = event.target.value;
    };
    document.getElementById("lighty").onchange = function(event) {
         lightPosition[1] = event.target.value;
    };
    document.getElementById("lightz").onchange = function(event) {
         lightPosition[2] =  event.target.value;
    };


    // Get and setup uniformVariables
    modelViewMatrixLoc = gl.getUniformLocation(program, "modelViewMatrix");

    projectionMatrix = ortho(-5, 5, -5, 5, -5, 5);

    normMatrix = mat4();

    normMatrixLoc = gl.getUniformLocation(program, "normalMatrix")
    
    


    modelViewMatrix = rotate(theta[Base], 0, 1, 0);
    render();
}

//----------------------------------------------------------------------------


function base() {

    // Position
    gl.enableVertexAttribArray(vCylinderLoc);
    gl.bindBuffer(gl.ARRAY_BUFFER, vCylinderBuffer);
    gl.vertexAttribPointer(vCylinderLoc, 4, gl.FLOAT, false, 0, 0);
    // Normal
    gl.enableVertexAttribArray(nCylinderLoc);
    gl.bindBuffer(gl.ARRAY_BUFFER, nCylinderBuffer);
    gl.vertexAttribPointer(nCylinderLoc, 3, gl.FLOAT, false, 0, 0);
    // Color
    gl.enableVertexAttribArray(cCylinderLoc);
    gl.bindBuffer(gl.ARRAY_BUFFER, cCylinderBuffer);
    gl.vertexAttribPointer(cCylinderLoc, 3, gl.FLOAT, false, 4 * Float32Array.BYTES_PER_ELEMENT, 0);


    var s = scale4(BASE_WIDTH, BASE_HEIGHT, BASE_WIDTH);
    var instanceMatrix = mult( translate( 0.0, 0.5 * BASE_HEIGHT, 0.0 ), s);
    var t = mult(modelViewMatrix, instanceMatrix);
    gl.uniformMatrix4fv(modelViewMatrixLoc,  false, flatten(t) );

    gl.drawArrays( gl.TRIANGLES, 0, cylinderPoints.length );
}

//----------------------------------------------------------------------------


function upperArm() {
    var s = scale4(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
    var instanceMatrix = mult(translate( 0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0 ),s);
    var t = mult(modelViewMatrix, instanceMatrix);
    gl.uniformMatrix4fv( modelViewMatrixLoc,  false, flatten(t) );
    gl.drawArrays( gl.TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------


function lowerArm()
{
    var s = scale4(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
    var instanceMatrix = mult( translate( 0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0 ), s);
    var t = mult(modelViewMatrix, instanceMatrix);
    gl.uniformMatrix4fv( modelViewMatrixLoc,  false, flatten(t) );
    gl.drawArrays( gl.TRIANGLES, 0, NumVertices );
}

//----------------------------------------------------------------------------
var rotationMatrix = mat4();
rotationMatrix = mult(rotationMatrix, rotateY(-0.15));
//rotationMatrix = mult(rotationMatrix, rotateZ(-0.05));
var render = function() {
    gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // Light Position
    gl.uniform3f(gl.getUniformLocation(program, "lightPosition"), lightPosition[0], lightPosition[1], lightPosition[2]);
    // Reflection Coefficients.
    gl.uniform1f(gl.getUniformLocation(program, "Ka"), KaVal);
    gl.uniform1f(gl.getUniformLocation(program, "Ks"), KsVal);
    gl.uniform1f(gl.getUniformLocation(program, "Kd"), KdVal);
    // Ambient Color Should be black
    gl.uniform3f(gl.getUniformLocation(program, "ambientColor"), ambientColor[0], ambientColor[1], ambientColor[2]);
    // Specular Color Should be white
    gl.uniform3f(gl.getUniformLocation(program, "specularColor"), specularColor[0], specularColor[1], specularColor[2]);
    // Shininess
    gl.uniform1f(gl.getUniformLocation(program, "shininess"), shininess);

    gl.uniformMatrix4fv( gl.getUniformLocation(program, "projectionMatrix"),  false, flatten(projectionMatrix) );

    modelViewMatrix = mult(modelViewMatrix, rotationMatrix);
    normMatrix = normalMatrix(modelViewMatrix, false);
    gl.uniformMatrix4fv( normMatrixLoc,  false, flatten(normMatrix) );

    base();

/**
    modelViewMatrix = mult(modelViewMatrix, translate(0.0, BASE_HEIGHT, 0.0));
    modelViewMatrix = mult(modelViewMatrix, rotate(theta[LowerArm], 0, 0, 1 ));
    normMatrix = normalMatrix(modelViewMatrix, false);
    //console.log(normMatrix);
    gl.uniformMatrix4fv( normMatrixLoc,  false, flatten(normMatrix) );
    lowerArm();

    modelViewMatrix  = mult(modelViewMatrix, translate(0.0, LOWER_ARM_HEIGHT, 0.0));
    modelViewMatrix  = mult(modelViewMatrix, rotate(theta[UpperArm], 0, 0, 1) );
    normMatrix = normalMatrix(modelViewMatrix, false);
    //console.log(normMatrix);
    gl.uniformMatrix4fv( normMatrixLoc,  false, flatten(normMatrix) );
    upperArm(); **/

    requestAnimationFrame(render);
}
requestAnimationFrame(render);

var changeView = function(){
    if(topView){
        topView = false;
    }else{
        topView = true;
    }
}