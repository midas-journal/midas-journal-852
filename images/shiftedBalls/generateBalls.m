function varargout = generateBalls(varargin)
%GENERATEBALLS  One-line description here, please.
%
%   output = generateBalls(input)
%
%   Example
%   generateBalls
%
%   See also
%
%
% ------
% Author: David Legland
% e-mail: david.legland@grignon.inra.fr
% Created: 2011-07-28,    using Matlab 7.9.0.529 (R2009b)
% Copyright 2011 INRA - Cepia Software Platform.


radii = sqrt(3)/2*power(4/3, 0:9);

nBalls = 100;

tic;
for r = 1:length(radii)
    r
    name = sprintf('ball_r%02d', r);
    
    radius = radii(r);
    dim = 2 * radius + 5;
    
    if ~exist(name, 'dir')
        mkdir(name);
    end
    
    for s = 1:nBalls
        
        center = rand(1, 3) + radius + 2;
    
        img = discreteBall(1:dim, 1:dim, 1:dim, [center radius]);
        
        fileName = sprintf('ball_r%02d_i%02d.tif', r, s-1);
        savestack(uint8(255*img), fullfile(name, fileName));
        
    end
end

toc
