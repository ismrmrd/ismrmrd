function [header] = deserialize(xmlstring)
%DESERIALIZE Summary of this function goes here
%   Detailed explanation goes here

% Get a parser
db = javax.xml.parsers.DocumentBuilderFactory.newInstance().newDocumentBuilder();

% Turn the string into a stream
isrc = org.xml.sax.InputSource();
isrc.setCharacterStream(java.io.StringReader(xmlstring))

% Parse it
dom = db.parse(isrc);

% Get the root element
rootNode = dom.getDocumentElement();

% Initialize an empty struct
header = struct;

% The header always has a few nodes (e.g. encoding is required)
childNodes = getChildNodes(rootNode);
numChildNodes = getLength(childNodes);

for n = 1:numChildNodes
    theChild = item(childNodes,n-1);
    name = char(getNodeName(theChild));
    
    if strcmp(name,'version')
        header.version = str2num(getTextContent(theChild));
    end

    if strcmp(name,'subjectInformation')
        header.subjectInformation = parseSubjectInformation(theChild);
    end
    
    if strcmp(name,'studyInformation')
        header.studyInformation = parseStudyInformation(theChild);
    end
    
    if strcmp(name,'measurementInformation')
        header.measurementInformation = parseMeasurementInformation(theChild);
    end
    
    if strcmp(name,'acquisitionSystemInformation')
        header.acquisitionSystemInformation = parseAcquisitionSystemInformation(theChild);
    end
    
    if strcmp(name,'experimentalConditions')
        header.experimentalConditions = parseExperimentalConditions(theChild);
    end

    if strcmp(name,'encoding')
        % there can be multiple encodings but there must be at least one
        if isfield(header, 'encoding')
            nenc = length(header.encoding);
        else
            nenc = 1;
        end
        header.encoding(nenc) = parseEncoding(theChild);
    end

    if strcmp(name,'sequenceParameters')
        header.sequenceParameters = parseSequenceParameters(theChild);
    end

    if strcmp(name,'userParameters')
        header.userParameters = parseUserParameters(theChild);
    end

end

end

% ----- Subfunction parseSubjectInformation -----
function subjectInformation = parseSubjectInformation(theNode)
    subjectInformation = struct;
end

% ----- Subfunction parseStudyInformation -----
function studyInformation = parseStudyInformation(theNode)
    studyInformation = struct;
end

% ----- Subfunction parseMeasurementInformation -----
function measurementInformation = parseMeasurementInformation(theNode)
    measurementInformation = struct;
end

% ----- Subfunction parseAcquisitionSystemInformation -----
function acquisitionSystemInformation = parseAcquisitionSystemInformation(theNode)
    acquisitionSystemInformation = struct;
end

% ----- Subfunction parseExperimentalConditions -----
function experimentalConditions = parseExperimentalConditions(theNode)

% Walk down the tree
childNodes = getChildNodes(theNode);
numChildNodes = getLength(childNodes);

experimentalConditions = struct;

for n = 1:numChildNodes
    theChild = item(childNodes,n-1);
    name = char(getNodeName(theChild));

    if strcmp(name,'H1resonanceFrequency_Hz')
        experimentalConditions.H1resonanceFrequency_Hz = str2num(getTextContent(theChild));
        break;
    end
end

end

% ----- Subfunction parseEncoding -----
function encoding = parseEncoding(theNode)
    encoding = struct;
end

% ----- Subfunction parseSequenceParameters -----
function sequenceParameters = parseSequenceParameters(theNode)
    sequenceParameters = struct;
end

% ----- Subfunction parseUserParameters -----
function userParameters = parseUserParameters(theNode)
    userParameters = struct;
end
