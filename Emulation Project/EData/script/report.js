var Utils = {
	/*Get the  readable name of a dataset key*/
	getName : function(key)
	{
		switch(key)
		{
			case 'pID': return 'packet ID';
			case 'pSize': return 'packet Size';
			case 'rID': return 'Router ID';
			case 'insT': return 'Insert Time';
			case 'exitT': return 'Exit Time';
			case 'totalT': return 'Total Time';
			case 'fil': return 'Fillage';
			case 'dp': return 'Drop Rate';
			case 'ue': return 'Total Time Until Exit';
			case 'bsz': return 'Buffer Size';
			case 'bus': return 'Buffer Used Size';
			case 'gaf' : return 'Average Fillage';
			case 'gadr': return 'Average DropRate';
			case 'asp' : return 'Average Speed';
			case 'te' : return 'At Time:';
			default : return '';
		}
		return '';
	},
	getPacketOffset : function()
	{
		for (var i=0;i<ts.length;++i)
		{
			if (ts[i].type !== 'RD') 
			{
				return i;
			}
		}
		return ts.length;
	}
}

/**
* parent_ DOMEelement : where to output the data to
* type String : determins if it's router Data ('RD') or packet Data ('PD')
* fromIndex int: the start of the graph data in the data set
* length int : the length of the graph data in the data set
*/
var DisplayData = function(parent_,type,fromIndex,length)
{
	var _packetDataOffset = 0; //holds the packet data offset in the data object
	var _data_len = 0; //the length of the data set
	


	var row = function(data)
	{
		var r = document.createElement('tr');
		r.setAttribute('class','row');
		for (var t in data)
		{
			if (t !== 'type')
			{
				var l = document.createElement('td');
				l.innerHTML = Utils.getName(t);
				l.setAttribute('class','label');
				var d = document.createElement('td');
				d.setAttribute('class',t);
				d.innerHTML = data[t];
				r.appendChild(l);
				r.appendChild(d);
			}
		}
		return r;
	}

	var init = function()
	{
		if (typeof ts === 'undefined') {return;}
		if (fromIndex < 0) {return;}
		if (length < 1) {return;}
		_data_len = ts.length;
		//calculate the packet data offset
		_packetDataOffset = Utils.getPacketOffset();
		var table = document.createElement('table');
		// table.setAttribute('border','1');
		if (type==='PD') {fromIndex+=_packetDataOffset;}
		//make sure it length doesn't go out of bounds
		var end = fromIndex+length > _data_len ? _data_len : fromIndex+length;
		for (var i=fromIndex;i<end;++i)
		{
			if (ts[i].type === type) {table.appendChild(row(ts[i]));}
		}
		parent_.appendChild(table);
		
	}();
}

/**
* parent_ DOMEelement : where to output the graph to
* name String : the name of the graph
* X String : the tag for the X-axis of the graph
* Y String : the tag fot the Y-axis of the graph
* resolution int: how many sampling to take from the dataset (size of 'length' means all of them)
* fromIndex int: the start of the graph data in the data set
* length int : the length of the graph data in the data set
*/
var MakeGraph = function(parent_,name,X,Y,resolution,fromIndex,length)
{

	var _cid = 'gd';
	var _packetOffset = Utils.getPacketOffset();
	var _data_len = 0;
	var _chart = null;
	var _dataset = [];
	var _labels = [];


	var gatherData = function()
	{
		var len  = ts.length;
		var sn = Math.floor(len / resolution);
		var end = fromIndex+length > _data_len ? _data_len : fromIndex+length;
		for (var i=(fromIndex+_packetOffset);i<end;++i)
		{
			//n samples of the data
			if ((i % sn)===0)
			{
				_labels.push(ts[i][X]);
				_dataset.push(ts[i][Y]);
			}
		}
	}

	var getGraphDataset = function()
	{
		var dataset = {
			'labels':_labels,
			'datasets':[
				{
					fillColor : "rgba(151,187,205,0.5)",
					strokeColor : "rgba(151,187,205,1)",
					pointColor : "rgba(151,187,205,1)",
					pointStrokeColor : "#fff",
					'data':_dataset
				} 
		]};
		return dataset;
	}

	var init = function()
	{
		if (resolution < 1) {return;}
		if (fromIndex < 0) {return;}
		if (length < 1) {return;}
		if (Utils.getName(X) === '') {return;}
		if (Utils.getName(Y) === '') {return;}
		_data_len = ts.length;
		_cid += X+Y;
		$(parent_).append("<h3>"+name+"</h3><canvas id='"+_cid+"' width='1000' height='500'></canvas>");
		var ctx = $("#"+_cid)[0].getContext("2d");
		_chart = new Chart(ctx);
		gatherData();
		_chart.Line(getGraphDataset());
	}();
}

var makeSection  = function(name)
{
	var sec = document.createElement('section');
	sec.setAttribute('id','name_d');
	sec.setAttribute('class','sec name');
	var title = document.createElement('h2');
	title.innerHTML = name;
	sec.appendChild(title);
	return sec;
}

$(function() {
	var $$ = $('body');
	$$.append("<h1>Emulation Report:</h1>");
	
	var router_section = makeSection('Routers Data');
	DisplayData(router_section,'RD',0,10);
	$$.append(router_section);

	var stat_section = makeSection('Satistics Data');
	DisplayData(stat_section,'SD',0,ts.length);
	var t = document.createElement('h3');
	t.innerHTML = '<blink>Emulation Overall Satistics TODO</blink>';
	stat_section.appendChild(t);
	$$.append(stat_section)

	//make the packets data visible
	var start = 0;
	var length = 1000;
	var packet_section = makeSection("Packet Data: (showing "+start+" - "+(start+length)+")");
	DisplayData(packet_section,'PD',start,length);
	$$.append(packet_section);	

	//make the graph section dynamic by properties
	var x_prop = 'pID'; //packetID
	var y_prop = 'dp'; //DropRate
	var graphs = makeSection("Graphs");	
	$$.append(graphs);
	MakeGraph(graphs,'Droprate/Packets',x_prop,y_prop,100,0,ts.length);
	MakeGraph(graphs,'Fillage/Packets',x_prop,'fil',100,0,ts.length);

});