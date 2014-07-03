using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class UseRenderingPlugin : MonoBehaviour {

	public int TargetMonitor;
	// Native plugin rendering events are only called if a plugin is used
	// by some script. This means we have to DllImport at least
	// one function in some active script.
	// For this example, we'll call into plugin's SetTimeFromUnity
	// function and pass the current time so the plugin can animate.
	[DllImport ("RenderingPlugin")]
	private static extern void SetTimeFromUnity (float t);

	[DllImport ("RenderingPlugin")]
	private static extern int GetMonitorResolutionX(int targ_mon);
	[DllImport ("RenderingPlugin")]
	private static extern int GetMonitorResolutionY(int targ_mon);

	[DllImport ("RenderingPlugin")]
	private static extern void LinkMonitor (int t,System.IntPtr texture);
	[DllImport ("RenderingPlugin")]
	private static extern void UnlinkMonitor (int t,System.IntPtr texture);
	
	IEnumerator Start () {
		CreateTextureAndPassToPlugin();
		yield return StartCoroutine("CallPluginAtEndOfFrames");
	}
	
	private void CreateTextureAndPassToPlugin()
	{
		int szx, szy;

		// Create a texture
		szx = GetMonitorResolutionX (TargetMonitor);
		szy = GetMonitorResolutionY (TargetMonitor);

		Texture2D tex = new Texture2D(szx,szy,TextureFormat.ARGB32,false);
		// Set point filtering just so we can see the pixels clearly
		tex.filterMode = FilterMode.Trilinear;//Point;
		// Call Apply() so it's actually uploaded to the GPU
		tex.Apply();
		
		// Set texture onto our matrial
		renderer.material.mainTexture = tex;

		// Pass texture pointer to the plugin
		LinkMonitor (TargetMonitor,tex.GetNativeTexturePtr());
	}
	
	private IEnumerator CallPluginAtEndOfFrames ()
	{
		while (true) {
			// Wait until all frame rendering is done
			yield return new WaitForEndOfFrame();
			
			// Set time for the plugin
			SetTimeFromUnity (Time.timeSinceLevelLoad);
			// Issue a plugin event with arbitrary integer identifier.
			// The plugin can distinguish between different
			// things it needs to do based on this ID.
			// For our simple plugin, it does not matter which ID we pass here.
			GL.IssuePluginEvent (1);
		}
	}

}
