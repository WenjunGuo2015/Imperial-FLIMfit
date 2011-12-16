function varargout = dataset_selection(varargin)
% DATASET_SELECTION MATLAB code for dataset_selection.fig
%      DATASET_SELECTION, by itself, creates a new DATASET_SELECTION or raises the existing
%      singleton*.
%
%      H = DATASET_SELECTION returns the handle to a new DATASET_SELECTION or the handle to
%      the existing singleton*.
%
%      DATASET_SELECTION('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in DATASET_SELECTION.M with the given input arguments.
%
%      DATASET_SELECTION('Property','Value',...) creates a new DATASET_SELECTION or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before dataset_selection_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to dataset_selection_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help dataset_selection

% Last Modified by GUIDE v2.5 01-Jul-2011 16:59:01

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @dataset_selection_OpeningFcn, ...
                   'gui_OutputFcn',  @dataset_selection_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before dataset_selection is made visible.
function dataset_selection_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to dataset_selection (see VARARGIN)

% Choose default command line output for dataset_selection
handles.output = hObject;

data = varargin{1};

if size(data,2) > size(data,1)
    data = data';
end

data = [num2cell(true(size(data))), data];
set(handles.dataset_table,'Data',data);

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes dataset_selection wait for user response (see UIRESUME)
uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = dataset_selection_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout = handles.output;

delete(handles.figure1);


% --- Executes on button press in select_all_pushbutton.
function select_all_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to select_all_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
data = get(handles.dataset_table,'Data');
data(:,1) = num2cell(true(size(data,1),1));
set(handles.dataset_table,'Data',data);
guidata(hObject,handles);


% --- Executes on button press in select_none_pushbutton.
function select_none_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to select_none_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
data = get(handles.dataset_table,'Data');
data(:,1) = num2cell(false(size(data,1),1));
set(handles.dataset_table,'Data',data);
guidata(hObject,handles);


% --- Executes on button press in ok_pushbutton.
function ok_pushbutton_Callback(hObject, eventdata, handles)
% hObject    handle to ok_pushbutton (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

data = get(handles.dataset_table,'Data');
sel = cell2mat(data(:,1));
data = data(:,2);
sel_idx = 1:size(data,1);

lazy_loading = get(handles.lazy_loading_checkbox,'Value');

handles.output = {data(sel), sel_idx(sel), lazy_loading};

guidata(hObject,handles);

uiresume(handles.figure1);


% --- Executes on button press in lazy_loading_checkbox.
function lazy_loading_checkbox_Callback(hObject, eventdata, handles)
% hObject    handle to lazy_loading_checkbox (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of lazy_loading_checkbox