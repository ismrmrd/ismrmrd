function img = simple_gridder(co,data,weight,matrix_size)
%
%   img = simple_gridder(co,data,weight,matrix_size)
%
%   A very simple gridder written purely in Matlab. 
%   Only tested for 2D
%
%   Input arguments:
%     - co [N,2], k-space coordinates in the range [-0.5:0.5]
%     - data, vector of N complex data points
%     - weight, vector of N data weights (density compensation)
%     - matrix_size, 2-element vector, e.g. [128 128]
%
%   Michael S. Hansen (michael.hansen@nih.gov), 2012
%
%

over_sampling = 2.0;
kernel_width = 8;
kernel_samples = 100000;
kernel_beta = 18.5547;
matrix_size_oversampled = matrix_size*over_sampling;

kernel_table = 1.0-(2.0*[-bitshift(kernel_samples,-1):(bitshift(kernel_samples,-1)-1)]/kernel_samples).^2;

kernel_table(kernel_table<0) = 0;
kernel_table=sqrt(kernel_table);
kernel_table = bessi0(kernel_beta .* kernel_table);
knorm = sum(kernel_table(:))/numel(kernel_table);
kernel_table = kernel_table ./ knorm;

grid_cells = ceil(co .* repmat(matrix_size_oversampled,size(co,1),1)-(kernel_width/2));
kernel_idx = grid_cells-(co .* repmat(matrix_size_oversampled,size(co,1),1));
grid_cells = uint32(grid_cells+repmat(bitshift(matrix_size_oversampled+kernel_width,-1),size(co,1),1));
kernel_idx = uint32(floor(((kernel_idx + kernel_width/2)/kernel_width)*kernel_samples));
kernel_step = uint32(floor(kernel_samples/kernel_width));

%Calculate deapodization
kern = -bitshift(kernel_width,-1):(-bitshift(kernel_width,-1)+kernel_width-1);
kern = 1.0-(2.0*kern/kernel_width).^2;
kern = bessi0(kernel_beta .* kern);
kern = repmat(kern,kernel_width,1) .* repmat(kern',1,kernel_width);
deapodization = zeros(matrix_size_oversampled);
deapodization((1:kernel_width) + bitshift(matrix_size_oversampled(1)-kernel_width,-1), ...
              (1:kernel_width) + bitshift(matrix_size_oversampled(2)-kernel_width,-1)) = kern;  
deapodization = fftshift(ifftn(ifftshift(deapodization))) ./ sqrt(numel(deapodization));

%Do convolution
oversampled_grid_padded = zeros(matrix_size_oversampled+kernel_width);
for y=1:(kernel_width),
    for x=1:(kernel_width),       
        oversampled_grid_padded = oversampled_grid_padded + ...
            accumarray([grid_cells(:,1)+(x-1),grid_cells(:,2)+(y-1)], ...
            weight(:).*data(:).*(kernel_table(kernel_idx(:,1)+(x-1)*kernel_step+1).*kernel_table(kernel_idx(:,2)+(y-1).*kernel_step+1))', ...
            matrix_size_oversampled+kernel_width);
    end
end

osps = size(oversampled_grid_padded);
oss = matrix_size_oversampled;

%Let's just get rid of the padding, it should really be folded in
oversampled_grid = oversampled_grid_padded(bitshift(osps(1)-oss(1),-1):bitshift(osps(1)-oss(1),-1)+matrix_size_oversampled(1)-1, ...
                                           bitshift(osps(2)-oss(2),-1):bitshift(osps(1)-oss(2),-1)+matrix_size_oversampled(2)-1); 

%FFT to image space                                       
img = fftshift(ifftn(ifftshift(oversampled_grid))) ./ sqrt(numel(oversampled_grid));

%Deapodization
img = img ./ deapodization;

%Remove oversampling
img = img((1:matrix_size(1))+bitshift(matrix_size_oversampled(1)-matrix_size(1),-1), ...
          (1:matrix_size(2))+bitshift(matrix_size_oversampled(2)-matrix_size(2),-1));

%TODO
% - fold padding back in

return

function ans = bessi0(x)

ax = abs(x);
ans = zeros(size(x));

% ax<3.75
k = find(ax<3.75);
y=x(k)./3.75;
y=y.^2;
ans(k)=1.0+y.*(3.5156229+y.*(3.0899424+y.*(1.2067492 ...
    +y.*(0.2659732+y.*(0.360768e-1+y.*0.45813e-2)))));

% ax>=3.75
k = find(ax>=3.75);
y=3.75./ax(k);
ans(k)=(exp(ax(k))./sqrt(ax(k))).*(0.39894228+y.*(0.1328592e-1 ...
    +y.*(0.225319e-2+y.*(-0.157565e-2+y.*(0.916281e-2 ...
    +y.*(-0.2057706e-1+y.*(0.2635537e-1+y.*(-0.1647633e-1 ...
    +y.*0.392377e-2))))))));
return