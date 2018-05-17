classdef Hdf5Test < matlab.unittest.TestCase
    
    properties
        dataset
        filename
    end
    
    methods (TestMethodSetup)
        
        function setPath(testCase)
            addpath('../../matlab');
        end
        function makeHdfFileName(testCase)
            testCase.filename = [tempname '.h5'];
            testCase.dataset = ismrmrd.Dataset(testCase.filename);
        end
    end
    
    methods (TestMethodTeardown)
        function removeHdfFile(testCase)
            delete(testCase.filename)
        end
    end
       
    methods (Test)
        
        function AddAcquisition(testCase)
                headers = ismrmrd.AcquisitionHeader(10);
                headers.version(:) = 1;
                headers.available_channels(:) = 32;
                headers.active_channels(:) = 32;
                headers.number_of_samples(:) = 128;
                
                acq = ismrmrd.Acquisition(headers);
                
                testCase.dataset.appendAcquisition(acq);
                
                acq2 = testCase.dataset.readAcquisition();
                
                testCase.verifyEqual(acq,acq2);
            
            
        end
        
        function AddWaveform(testCase)
            headers = ismrmrd.WaveformHeader(10);
            headers.version(:) = 1;
            headers.channels(:) = 4;
            headers.number_of_samples(:) = 40;
            
            wav = ismrmrd.Waveform(headers);
            
            testCase.dataset.appendWaveform(wav);
            
            wav2 = testCase.dataset.readWaveform();
            
            testCase.verifyEqual(wav,wav2);
        end
        
    end
end
