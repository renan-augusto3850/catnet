var isOpen = false;
var isHide = true;
class Smart {
    constructor(){
        this.appName = "";
        this.username = "";
        this.isDragging = false;
        this.offsetX = 50;
        this.offsetY = 50;
        this.windowElement;
        this.titleBarElement;
    }
    openAppDrawer() {
        document.getElementById("app-drawer").style.bottom = "80px";
    }
    closeAppDrawer() {
        document.getElementById("app-drawer").style.bottom = "-100%";
    }
    showSuspendedMenu(){
        document.getElementById('suspended-menu').style.display = 'block';
    }
    hideSuspendedMenu(){
        document.getElementById('suspended-menu').style.display = 'none';
    }
    inicializeWindow(appName, width, height) {
        this.appName = appName;
        document.querySelector(".layout").innerHTML += `<div class="window" id="window-${appName}">
            <div class="TopofWindow" id="TopofWindow-${appName}" style='width: ${width + 2};'>
                <button id="closebutton-${appName}" class='closebutton'>X</button>
                <p class="namefromapp" id="nameapp">${appName}</p></div>
            <div class="window-into" id="window-into-${appName}" style='width: ${width}; height: ${height};'>

            </div>
        </div>`
        this.windowElement = document.getElementById("window-" + this.appName);
        document.querySelector(".app-drawer").style.bottom = "-100%";
        isOpen = false;
        this.titleBarElement = document.getElementById('TopofWindow-' + this.appName);
        this.titleBarElement.addEventListener('mousedown', startDragging);
        this.titleBarElement.addEventListener('mouseup', stopDragging);

        function startDragging(e) {
            console.log("dragging ...");
            this.isDragging = true;
            this.offsetX = e.clientX - e.target.getBoundingClientRect().left;
            this.offsetY = e.clientY - e.target.getBoundingClientRect().top;
            e.target.style.cursor = 'grabbing'; // Change cursor style during drag
        }

        function stopDragging(e) {
            this.isDragging = false;
            e.target.style.cursor = 'grab'; // Restore cursor style
        }

        document.addEventListener('mousemove', (e) => {
            if (this.isDragging === true) {
                console.log("Nice...");
                e.preventDefault();
                const newX = e.clientX - this.offsetX;
                const newY = e.clientY - this.offsetY;
                e.target.style.left = `${newX}px`;
                e.target.style.top = `${newY}px`;
            }
        });
    }
    addTexttoWindow(text, Elementclass, type) {
        if(type === "bold"){
            document.getElementById("window-into-" + this.appName).innerHTML += `<h1 class='${Elementclass}'>${text}</h1>`;
        } else {
            document.getElementById("window-into-" + this.appName).innerHTML += `<p class='${Elementclass}'>${text}</p>`;
        }
    }
    addImagetoWindow(Elementsource, Elementclass, width, height) {
        document.getElementById("window-into-" + this.appName).innerHTML += `<img src='${Elementsource}' class='${Elementclass}' style='width: ${width}; heigth: ${height};'>`;
    }
    addButtontoWindow(text, Elementid, width, height) {
        document.getElementById("window-into-" + this.appName).innerHTML += `<br> <button id='${Elementid}' style='width: ${width}; heigth: ${height};'>${text}</button>`;
    }
    addInlineButtontoWindow(text, Elementid, width, height) {
        document.getElementById("window-into-" + this.appName).innerHTML += `<button id='${Elementid}' style='width: ${width}; heigth: ${height};'>${text}</button>`;
    }
    windowBackgroundColor(color) {
        document.getElementById("window-into-" + this.appName).style.backgroundColor = color;
    }
    windowColor(color) {
        document.getElementById("window-into-" + this.appName).style.color = color;
    }
    addHTML(code) {
        document.getElementById("window-into-" + this.appName).innerHTML += code;
    }
    innerHTML(code) {
        document.getElementById("window-into-" + this.appName).innerHTML = code;
    }
    close(appName) {
        document.getElementById("closebutton-" + appName).addEventListener("click", function() {
            document.getElementById("window-" + appName).remove();
        });
    }
}
var smart = new Smart();
document.getElementById("open-drawer").addEventListener("click", function(){
    if(isOpen === false){
        smart.openAppDrawer();
        isOpen = true;
    } else{
        smart.closeAppDrawer();
        isOpen = false;
    }
});
document.getElementById("show-menu").addEventListener("click", function(){
    if(isHide === true){
        smart.showSuspendedMenu();
        isHide = false;
    } else{
        smart.hideSuspendedMenu();
        isHide = true;
    }
});