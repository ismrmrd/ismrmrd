[xmlhdr data] = testread('~/Projects/ismrmrd_data/testdata.h5');

nacq = size(data,2);
k = zeros([data(1).head.number_of_samples nacq]);
for p = 1:nacq; 
    k(:,p) = data(p).data;
end

imagesc(abs(fftshift(ifft2(fftshift(k)))))
axis image; axis off;